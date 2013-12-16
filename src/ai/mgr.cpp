/**
 * @file src/ai/mgr.cpp
 * @project The Battle for Wesnoth NG - https://github.com/justinzane/wesnoth-ng
 * @brief 
 * @authors 
 * @copyright Copyright (C) 2009 - 2013 by Yurii Chernyi <terraninfo@terraninfo.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * Managing the AI lifecycle and interface for the rest of Wesnoth
 * @file
 */

#include "composite/ai.hpp"
#include "configuration.hpp"
#include "contexts.hpp"
#include "default/ai.hpp"
#include "mgr.hpp"
#include "formula/ai.hpp"
#include "registry.hpp"
#include "../game_events/pump.hpp"
#include "../log/log.hpp"
#include "../log/log.hpp"
#include "../serdes/string_utils.hpp"
#include "composite/component.hpp"

#include "global.hpp"
#include <boost/foreach.hpp>

#include <map>
#include <stack>
#include <vector>
#include "composite/engine.hpp"

namespace ai {

const std::string mgr::AI_TYPE_COMPOSITE_AI = "composite_ai";
const std::string mgr::AI_TYPE_SAMPLE_AI = "sample_ai";
const std::string mgr::AI_TYPE_IDLE_AI = "idle_ai";
const std::string mgr::AI_TYPE_FORMULA_AI = "formula_ai";
const std::string mgr::AI_TYPE_DEFAULT = "default";


static lg::log_domain log_ai_mgr("ai/mgr");
#define DBG_AI_mgr LOG_STREAM(debug, log_ai_mgr)
#define LOG_AI_mgr LOG_STREAM(info, log_ai_mgr)
#define ERR_AI_mgr LOG_STREAM(err, log_ai_mgr)

static lg::log_domain log_ai_mod("ai/mod");
#define DBG_AI_MOD LOG_STREAM(debug, log_ai_mod)
#define LOG_AI_MOD LOG_STREAM(info, log_ai_mod)
#define WRN_AI_MOD LOG_STREAM(warn, log_ai_mod)
#define ERR_AI_MOD LOG_STREAM(err, log_ai_mod)

holder::holder( side_number side, const config &cfg )
	: ai_(), side_context_(nullptr), readonly_context_(nullptr), readwrite_context_(nullptr), default_ai_context_(nullptr), side_(side), cfg_(cfg)
{
	DBG_AI_mgr << describe_ai() << "Preparing new AI holder" << std::endl;
}


void holder::init( side_number side )
{
	if (side_context_ == nullptr) {
		side_context_ = new side_context_impl(side,cfg_);
	} else {
		side_context_->set_side(side);
	}
	if (readonly_context_ == nullptr){
		readonly_context_ = new readonly_context_impl(*side_context_,cfg_);
		readonly_context_->on_readonly_context_create();
	}
	if (readwrite_context_ == nullptr){
		readwrite_context_ = new readwrite_context_impl(*readonly_context_,cfg_);
	}
	if (default_ai_context_ == nullptr){
		default_ai_context_ = new default_ai_context_impl(*readwrite_context_,cfg_);
	}
	if (!this->ai_){
		ai_ = boost::shared_ptr<ai_composite>(new ai_composite(*default_ai_context_,cfg_));
	}

	if (this->ai_) {
		ai_->on_create();
		foreach_ng(config &mod_ai, cfg_.child_range("modify_ai")) {
			if (!mod_ai.has_attribute("side")) {
				mod_ai["side"] = side;
			}
			modify_ai(mod_ai);
		}
		cfg_.clear_children("modify_ai");

		std::vector<engine_ptr> engines = ai_->get_engines();
		for (std::vector<engine_ptr>::iterator it = engines.begin(); it != engines.end(); ++it)
		{
			(*it)->set_ai_context(&(ai_->get_ai_context()));
		}

	} else {
		ERR_AI_mgr << describe_ai()<<"AI lazy initialization error!" << std::endl;
	}

}

holder::~holder()
{
	if (this->ai_) {
		LOG_AI_mgr << describe_ai() << "Managed AI will be deleted" << std::endl;
	}
	delete this->default_ai_context_;
	delete this->readwrite_context_;
	delete this->readonly_context_;
	delete this->side_context_;
}


interface& holder::get_ai_ref()
{
	if (!this->ai_) {
		this->init(this->side_);
	}
	assert(this->ai_);

	return *this->ai_;
}


void holder::modify_ai_config_old( const config::const_child_itors &ai_parameters)
{
	// only handle aspects
	// transform ai_parameters to new-style config

	config cfg;
	configuration::upgrade_aspect_configs_from_1_07_02_to_1_07_03(this->side_,ai_parameters,cfg);
	//at this point we have a single config which contains [aspect][facet] tags
	DBG_AI_mgr << "after transforming [modify_side][ai] into new syntax, config contains:"<< std::endl << cfg << std::endl;

	if (this->readonly_context_ == nullptr) {
		// if not initialized, append that config to the bottom of base cfg
		// then, merge aspects with the same id
		cfg_.merge_with(cfg);
		cfg_.merge_children_by_attribute("aspect","id");
	} else {
		// else run 'add_facet' command on each [aspect][facet]
		foreach_ng(const config &cfg_a, cfg.child_range("aspect")) {
			foreach_ng(const config &cfg_f, cfg_a.child_range("facet")) {
				readonly_context_->add_facet(cfg_a["id"],cfg_f);
			}
		}
	}
}


void holder::modify_ai(const config &cfg)
{
	if (!this->ai_) {
		// if not initialized, initialize now.
		get_ai_ref();
	}
	const std::string &act = cfg["action"];
	LOG_AI_MOD << "side "<< side_ << "        [modify_ai] "<<act<<" \""<<cfg["path"]<<"\""<<std::endl;
	DBG_AI_MOD << std::endl << cfg << std::endl;
	DBG_AI_MOD << "side "<< side_ << " before [modify_ai]"<<std::endl << to_config() << std::endl;
	bool res = false;
	if (act == "add") {
		res = component_mgr::add_component(&*this->ai_,cfg["path"],cfg);
	} else if (act == "change") {
		res = component_mgr::change_component(&*this->ai_,cfg["path"],cfg);
	} else if (act == "delete") {
		res = component_mgr::delete_component(&*this->ai_,cfg["path"]);
	} else if (act == "try_delete") {
		res = component_mgr::delete_component(&*this->ai_,cfg["path"]);
		if (!res) {
			LOG_AI_MOD << "[modify_ai] "<<act<<" failed, ignoring because it's a try_delete"<< std::endl;
			res = true;
		}
	} else {
		ERR_AI_MOD << "modify_ai tag has invalid 'action' attribute " << act << std::endl;
	}
	DBG_AI_MOD << "side "<< side_ << "  after [modify_ai]"<<act<<std::endl << to_config() << std::endl;
	if (!res) {
		LOG_AI_MOD << "[modify_ai] "<<act<<" failed"<< std::endl;
	} else {
		LOG_AI_MOD << "[modify_ai] "<<act<<" success"<< std::endl;
	}

}

config holder::to_config() const
{
	if (!this->ai_) {
		return cfg_;
	} else {
		config cfg = ai_->to_config();
		cfg["version"] = "10703";
		if (this->side_context_!=nullptr) {
			cfg.merge_with(this->side_context_->to_side_context_config());
		}
		if (this->readonly_context_!=nullptr) {
			cfg.merge_with(this->readonly_context_->to_readonly_context_config());
		}
		if (this->readwrite_context_!=nullptr) {
			cfg.merge_with(this->readwrite_context_->to_readwrite_context_config());
		}
		if (this->default_ai_context_!=nullptr) {
			cfg.merge_with(this->default_ai_context_->to_default_ai_context_config());
		}

		return cfg;
	}
}



const std::string holder::describe_ai()
{
	std::string sidestr = lexical_cast<std::string>(this->side_);

	if (this->ai_!=nullptr) {
		return this->ai_->describe_self()+std::string(" for side ")+sidestr+std::string(" : ");
	} else {
		return std::string("not initialized ai with id=[")+cfg_["id"]+std::string("] for side ")+sidestr+std::string(" : ");
	}
}


const std::string holder::get_ai_overview()
{
	if (!this->ai_) {
		get_ai_ref();
	}
	std::stringstream s;
	s << "advancements:  " << this->ai_->get_advancements().get_value() << std::endl;
	s << "aggression:  " << this->ai_->get_aggression() << std::endl;
	s << "attack_depth:  " << this->ai_->get_attack_depth() << std::endl;
	s << "caution:  " << this->ai_->get_caution() << std::endl;
	s << "grouping:  " << this->ai_->get_grouping() << std::endl;
	s << "leader_aggression:  " << this->ai_->get_leader_aggression() << std::endl;
	s << "leader_ignores_keep:  " << this->ai_->get_leader_ignores_keep() << std::endl;
	s << "leader_value:  " << this->ai_->get_leader_value() << std::endl;
	s << "number_of_possible_recruits_to_force_recruit:  " << this->ai_->get_number_of_possible_recruits_to_force_recruit() << std::endl;
	s << "passive_leader:  " << this->ai_->get_passive_leader() << std::endl;
	s << "passive_leader_shares_keep:  " << this->ai_->get_passive_leader_shares_keep() << std::endl;
	s << "recruitment_diversity:  " << this->ai_->get_recruitment_diversity() << std::endl;
	s << "recruitment_ignore_bad_combat:  " << this->ai_->get_recruitment_ignore_bad_combat() << std::endl;
	s << "recruitment_ignore_bad_movement:  " << this->ai_->get_recruitment_ignore_bad_movement() << std::endl;
	s << "recruitment_instructions:  " << std::endl << "----config begin----" << std::endl;
	s << this->ai_->get_recruitment_instructions() << "-----config end-----" << std::endl;
	s << "recruitment_more:  " << utils::join(this->ai_->get_recruitment_more()) << std::endl;
	s << "recruitment_pattern:  " << utils::join(this->ai_->get_recruitment_pattern()) << std::endl;
	s << "recruitment_randomness:  " << this->ai_->get_recruitment_randomness() << std::endl;
	s << "recruitment_save_gold:  " << std::endl << "----config begin----" << std::endl;
	s << this->ai_->get_recruitment_save_gold() << "-----config end-----" << std::endl;
	s << "scout_village_targeting:  " << this->ai_->get_scout_village_targeting() << std::endl;
	s << "simple_targeting:  " << this->ai_->get_simple_targeting() << std::endl;
	s << "support_villages:  " << this->ai_->get_support_villages() << std::endl;
	s << "village_value:  " << this->ai_->get_village_value() << std::endl;
	s << "villages_per_scout:  " << this->ai_->get_villages_per_scout() << std::endl;

	return s.str();
}



const std::string holder::get_ai_structure()
{
	if (!this->ai_) {
		get_ai_ref();
	}
	return component_mgr::print_component_tree(&*this->ai_,"");
}


const std::string holder::get_ai_identifier() const
{
	return cfg_["id"];
}

component* holder::get_component(component *root, const std::string &path) {
	if (!game_config::debug) // Debug guard
	{
		return nullptr;
	}

	if (root == nullptr) // Return root component(ai_)
	{
		if (!this->ai_) {
			this->init(this->side_);
		}
		assert(this->ai_);

		return &*this->ai_;
	}

	return component_mgr::get_component(root, path);
}

// =======================================================================
// LIFECYCLE
// =======================================================================


mgr::AI_map_of_stacks mgr::ai_map_;
game_info *mgr::ai_info_;
events::generic_event mgr::user_interact_("ai_user_interact");
events::generic_event mgr::sync_network_("ai_sync_network");
events::generic_event mgr::gamestate_changed_("ai_gamestate_changed");
events::generic_event mgr::turn_started_("ai_turn_started");
events::generic_event mgr::recruit_list_changed_("ai_recruit_list_changed");
events::generic_event mgr::map_changed_("ai_map_changed");
int mgr::last_interact_ = 0;
int mgr::num_interact_ = 0;


void mgr::set_ai_info(const game_info& i)
{
	if (ai_info_!=nullptr){
		clear_ai_info();
	}
	ai_info_ = new game_info(i);
	registry::init();
}


void mgr::clear_ai_info(){
	delete ai_info_;
	ai_info_ = nullptr;
}


void mgr::add_observer( evt_observer* event_observer){
	user_interact_.attach_handler(event_observer);
	sync_network_.attach_handler(event_observer);
	turn_started_.attach_handler(event_observer);
	gamestate_changed_.attach_handler(event_observer);
}


void mgr::remove_observer(evt_observer* event_observer){
	user_interact_.detach_handler(event_observer);
	sync_network_.detach_handler(event_observer);
	turn_started_.detach_handler(event_observer);
	gamestate_changed_.detach_handler(event_observer);
}


void mgr::add_gamestate_observer( evt_observer* event_observer){
	gamestate_changed_.attach_handler(event_observer);
	turn_started_.attach_handler(event_observer);
	map_changed_.attach_handler(event_observer);
}


void mgr::remove_gamestate_observer(evt_observer* event_observer){
	gamestate_changed_.detach_handler(event_observer);
	turn_started_.detach_handler(event_observer);
	map_changed_.detach_handler(event_observer);
}



void mgr::add_map_changed_observer( evt_observer* event_observer )
{
	map_changed_.attach_handler(event_observer);
}


void mgr::add_recruit_list_changed_observer( evt_observer* event_observer )
{
	recruit_list_changed_.attach_handler(event_observer);
}


void mgr::add_turn_started_observer( evt_observer* event_observer )
{
	turn_started_.attach_handler(event_observer);
}


void mgr::remove_recruit_list_changed_observer( evt_observer* event_observer )
{
	recruit_list_changed_.detach_handler(event_observer);
}


void mgr::remove_map_changed_observer( evt_observer* event_observer )
{
	map_changed_.detach_handler(event_observer);
}


void mgr::remove_turn_started_observer( evt_observer* event_observer )
{
	turn_started_.detach_handler(event_observer);
}

void mgr::raise_user_interact() {
        const int interact_time = 30;
        const int time_since_interact = SDL_GetTicks() - last_interact_;
        if(time_since_interact < interact_time) {
                return;
        }

	++num_interact_;
        user_interact_.notify_observers();

        last_interact_ = SDL_GetTicks();

}

void mgr::raise_sync_network() {
	sync_network_.notify_observers();
}


void mgr::raise_gamestate_changed() {
	gamestate_changed_.notify_observers();
}


void mgr::raise_turn_started() {
	turn_started_.notify_observers();
}


void mgr::raise_recruit_list_changed() {
	recruit_list_changed_.notify_observers();
}


void mgr::raise_map_changed() {
	map_changed_.notify_observers();
}


// =======================================================================
// EVALUATION
// =======================================================================

const std::string mgr::evaluate_command( side_number side, const std::string& str )
{
	//insert new command into history
	history_.push_back(command_history_item(history_item_counter_++,str));

	//prune history - erase 1/2 of it if it grows too large
	if (history_.size()>MAX_HISTORY_SIZE){
		history_.erase(history_.begin(),history_.begin()+MAX_HISTORY_SIZE/2);
		LOG_AI_mgr << "AI mgr: pruned history" << std::endl;
	}

	if (!should_intercept(str)){
		interface& ai = get_active_ai_for_side(side);
		raise_gamestate_changed();
		return ai.evaluate(str);
	}

	return internal_evaluate_command(side,str);
}


bool mgr::should_intercept( const std::string& str )
{
	if (str.length()<1) {
		return false;
	}
	if (str.at(0)=='!'){
		return true;
	}
	if (str.at(0)=='?'){
		return true;
	}
	return false;

}

std::deque< command_history_item > mgr::history_;
long mgr::history_item_counter_ = 1;

//this is stub code to allow testing of basic 'history', 'repeat-last-command', 'add/remove/replace ai' capabilities.
//yes, it doesn't look nice. but it is usable.
//to be refactored at earliest opportunity
///@todo 1.9 extract to separate class which will use fai or lua parser
const std::string mgr::internal_evaluate_command( side_number side, const std::string& str ){
	const int MAX_HISTORY_VISIBLE = 30;

	//repeat last command
	if (str=="!") {
			//this command should not be recorded in history
			if (!history_.empty()){
				history_.pop_back();
				history_item_counter_--;
			}

			if (history_.empty()){
				return "AI mgr: empty history";
			}
			return evaluate_command(side, history_.back().get_command());//no infinite loop since '!' commands are not present in history
	};
	//show last command
	if (str=="?") {
		//this command should not be recorded in history
		if (!history_.empty()){
			history_.pop_back();
			history_item_counter_--;
		}

		if (history_.empty()){
			return "AI mgr: History is empty";
		}

		int n = std::min<int>( MAX_HISTORY_VISIBLE, history_.size() );
		std::stringstream strstream;
		strstream << "AI mgr: History - last "<< n <<" commands:\n";
		std::deque< command_history_item >::reverse_iterator j = history_.rbegin();

		for (int cmd_id=n; cmd_id>0; --cmd_id){
			strstream << j->get_number() << "    :" << j->get_command() << '\n';
			++j;//this is *reverse* iterator
		}

		return strstream.str();
	};


	std::vector< std::string > cmd = utils::parenthetical_split(str, ' ',"'","'");

	if (cmd.size()==3){
		//!add_ai side file
		if (cmd.at(0)=="!add_ai"){
			side_number side = lexical_cast<side_number>(cmd.at(1));
			std::string file = cmd.at(2);
			if (add_ai_for_side_from_file(side,file,false)){
				return std::string("AI mgr: added [")+mgr::get_active_ai_identifier_for_side(side)+std::string("] AI for side ")+lexical_cast<std::string>(side)+std::string(" from file ")+file;
			} else {
				return std::string("AI mgr: failed attempt to add AI for side ")+lexical_cast<std::string>(side)+std::string(" from file ")+file;
			}
		}
		//!replace_ai side file
		if (cmd.at(0)=="!replace_ai"){
			side_number side = lexical_cast<side_number>(cmd.at(1));
			std::string file = cmd.at(2);
			if (add_ai_for_side_from_file(side,file,true)){
					return std::string("AI mgr: added [")+mgr::get_active_ai_identifier_for_side(side)+std::string("] AI for side ")+lexical_cast<std::string>(side)+std::string(" from file ")+file;
			} else {
					return std::string("AI mgr: failed attempt to add AI for side ")+lexical_cast<std::string>(side)+std::string(" from file ")+file;
			}
		}

	} else if (cmd.size()==2){
		//!remove_ai side
		if (cmd.at(0)=="!remove_ai"){
			side_number side = lexical_cast<side_number>(cmd.at(1));
			remove_ai_for_side(side);
			return std::string("AI mgr: made an attempt to remove AI for side ")+lexical_cast<std::string>(side);
		}
		if (cmd.at(0)=="!"){
			//this command should not be recorded in history
			if (!history_.empty()){
				history_.pop_back();
				history_item_counter_--;
			}

			int command = lexical_cast<int>(cmd.at(1));
			std::deque< command_history_item >::reverse_iterator j = history_.rbegin();
			//yes, the iterator could be precisely positioned (since command numbers go 1,2,3,4,..). will do it later.
			while ( (j!=history_.rend()) && (j->get_number()!=command) ){
				++j;// this is *reverse* iterator
			}
			if (j!=history_.rend()){
				return evaluate_command(side,j->get_command());//no infinite loop since '!' commands are not present in history
			}
			return "AI mgr: no command with requested number found";
		}
	} else if (cmd.size()==1){
		if (cmd.at(0)=="!help") {
			return
				"known commands:\n"
				"!    - repeat last command (? and ! do not count)\n"
				"! NUMBER    - repeat numbered command\n"
				"?    - show a history list\n"
				"!add_ai TEAM FILE    - add a AI to side (0 - command AI, N - AI for side #N) from file\n"
				"!remove_ai TEAM    - remove AI from side (0 - command AI, N - AI for side #N)\n"
				"!replace_ai TEAM FILE    - replace AI of side (0 - command AI, N - AI for side #N) from file\n"
				"!help    - show this help message";
		}
	}


	return "AI mgr: nothing to do";
}

// =======================================================================
// ADD, CREATE AIs, OR LIST AI TYPES
// =======================================================================

///@todo 1.9 add error reporting
bool mgr::add_ai_for_side_from_file( side_number side, const std::string& file, bool replace )
{
	config cfg;
	if (!configuration::get_side_config_from_file(file,cfg)){
		ERR_AI_mgr << " unable to read [SIDE] config for side "<< side << "from file [" << file <<"]"<< std::endl;
		return false;
	}
	return add_ai_for_side_from_config(side,cfg,replace);
}


bool mgr::add_ai_for_side_from_config( side_number side, const config& cfg, bool replace ){
	config parsed_cfg;
	configuration::parse_side_config(side, cfg, parsed_cfg);

	if (replace) {
		remove_ai_for_side(side);
	}

	holder new_holder(side,parsed_cfg);
	std::stack<holder>& ai_stack_for_specific_side = get_or_create_ai_stack_for_side(side);
	ai_stack_for_specific_side.push(new_holder);
	return true;
}


///@todo 1.9 add error reporting
bool mgr::add_ai_for_side( side_number side, const std::string& ai_algorithm_type, bool replace )
{
	if (replace) {
		remove_ai_for_side (side);
	}
	config cfg;
	cfg["ai_algorithm"] = ai_algorithm_type;
	holder new_holder(side,cfg);
	std::stack<holder>& ai_stack_for_specific_side = get_or_create_ai_stack_for_side(side);
	ai_stack_for_specific_side.push(new_holder);
	return true;
}


ai_ptr mgr::create_transient_ai(const std::string &ai_algorithm_type, const config &cfg, ai_context *ai_context )
{
	assert(ai_context!=nullptr);

	//to add your own ai, register it in registry,cpp
	ai_factory::factory_map::iterator aii = ai_factory::get_list().find(ai_algorithm_type);
	if (aii == ai_factory::get_list().end()){
		aii = ai_factory::get_list().find("");
		if (aii == ai_factory::get_list().end()){
			throw game::game_error("no default ai set!");
		}
	}
	LOG_AI_mgr << "Creating new AI of type [" << ai_algorithm_type << "]"<< std::endl;
	ai_ptr new_ai = aii->second->get_new_instance(*ai_context,cfg);
	return new_ai;
}


// =======================================================================
// REMOVE
// =======================================================================

void mgr::remove_ai_for_side( side_number side )
{
	std::stack<holder>& ai_stack_for_specific_side = get_or_create_ai_stack_for_side(side);
	if (!ai_stack_for_specific_side.empty()){
		ai_stack_for_specific_side.pop();
	}
}


void mgr::remove_all_ais_for_side( side_number side )
{
	std::stack<holder>& ai_stack_for_specific_side = get_or_create_ai_stack_for_side(side);

	//clear the stack. std::stack doesn't have a '.clear()' method to do it
	while (!ai_stack_for_specific_side.empty()){
			ai_stack_for_specific_side.pop();
	}
}


void mgr::clear_ais()
{
	ai_map_.clear();
}

// =======================================================================
// Work with active AI parameters
// =======================================================================

void mgr::modify_active_ai_config_old_for_side ( side_number side, const config::const_child_itors &ai_parameters )
{
	get_active_ai_holder_for_side(side).modify_ai_config_old(ai_parameters);
}


void mgr::modify_active_ai_for_side ( side_number side, const config &cfg )
{
	if (ai_info_==nullptr) {
		//replay ?
		return;
	}
	get_active_ai_holder_for_side(side).modify_ai(cfg);
}


std::string mgr::get_active_ai_overview_for_side( side_number side)
{
	return get_active_ai_holder_for_side(side).get_ai_overview();
}


std::string mgr::get_active_ai_structure_for_side( side_number side)
{
	return get_active_ai_holder_for_side(side).get_ai_structure();
}


std::string mgr::get_active_ai_identifier_for_side( side_number side )
{
	return get_active_ai_holder_for_side(side).get_ai_identifier();
}

ai::holder& mgr::get_active_ai_holder_for_side_dbg(side_number side)
{
	if (!game_config::debug)
	{
		return *(new ai::holder(side, config()));
	}
	return get_active_ai_holder_for_side(side);
}


config mgr::to_config( side_number side )
{
	return get_active_ai_holder_for_side(side).to_config();
}


game_info& mgr::get_active_ai_info_for_side( side_number /*side*/ )
{
	return *ai_info_;
}


game_info& mgr::get_ai_info()
{
	return *ai_info_;
}


// =======================================================================
// PROXY
// =======================================================================

void mgr::play_turn( side_number side ){
	last_interact_ = 0;
	num_interact_ = 0;
	const int turn_start_time = SDL_GetTicks();
	/*hack. @todo 1.9 rework via extended event system*/
	get_ai_info().recent_attacks.clear();
	interface& ai_obj = get_active_ai_for_side(side);
	game_events::fire("ai turn");
	raise_turn_started();
	ai_obj.new_turn();
	ai_obj.play_turn();
	const int turn_end_time= SDL_GetTicks();
	DBG_AI_mgr << "side " << side << ": number of user interactions: "<<num_interact_<<std::endl;
	DBG_AI_mgr << "side " << side << ": total turn time: "<<turn_end_time - turn_start_time << " ms "<< std::endl;
}


// =======================================================================
// PRIVATE
// =======================================================================
// =======================================================================
// AI STACKS
// =======================================================================
std::stack<holder>& mgr::get_or_create_ai_stack_for_side( side_number side )
{
	AI_map_of_stacks::iterator iter = ai_map_.find(side);
	if (iter!=ai_map_.end()){
		return iter->second;
	}
	return ai_map_.insert(std::make_pair(side, std::stack<holder>())).first->second;
}

// =======================================================================
// AI HOLDERS
// =======================================================================
holder& mgr::get_active_ai_holder_for_side( side_number side )
{
	std::stack<holder>& ai_stack_for_specific_side = get_or_create_ai_stack_for_side(side);

	if (!ai_stack_for_specific_side.empty()){
		return ai_stack_for_specific_side.top();
	} else {
		config cfg = configuration::get_default_ai_parameters();
		holder new_holder(side, cfg);
		ai_stack_for_specific_side.push(new_holder);
		return ai_stack_for_specific_side.top();
	}
}

// =======================================================================
// AI POINTERS
// =======================================================================

interface& mgr::get_active_ai_for_side( side_number side )
{
	return get_active_ai_holder_for_side(side).get_ai_ref();
}


// =======================================================================
// MISC
// =======================================================================

} //end of namespace ai
