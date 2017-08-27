//============================================================================
//
// ***** work on this file is optional, and might only be necessary for bonus work *****
//
//% Student Name 1: student1
//% Student 1 #: 123456781
//% Student 1 userid (email): stu1 (stu1@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put �none� if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put �none�)__
//
// Also, list any resources beyond the course textbook and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Enter the above information in tokenClasses.hpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "PP5_<userid1>_<userid2>" (eg. PP5_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClasses.cpp
// Description : Course Project Part 5 (Interpreting/Evaluation)
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#include "tokenClasses.hpp"

// I'm not sure if next line works with other than GNU compiler
#include <cxxabi.h>

using namespace std;

namespace ensc251 {

// global variables
unsigned int tracker = 0; // tracker index
std::vector<Token*> tokenObjectPs; // objects for abstract syntax tree
evalMapType evalMap; // map holding evaluation results

char *demangle(const char *typeName) {
	int status;
	// I'm not sure if the below line works with compilers other than GNU
	return abi::__cxa_demangle(typeName, 0, 0, &status);
}

void print_evaluation_map()
{
	OUT<<std::endl<<"-----Evaluation Map-----"<< std::endl;
	for(auto it = evalMap.cbegin(); it != evalMap.cend(); ++it)
	{
		OUT << it->first << "     =     " << it->second.sptc->getStringValue()  << std::endl;
	}
	OUT << std::endl;
}

Token* postfix_exp()
{
	int old_tracker = tracker;

	auto subTreeP = tokenObjectPs[tracker]->process_id();
	if (subTreeP) {
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->process_postfix_operator()))
		{
				tObjP->add_childP(subTreeP);
				subTreeP = tObjP;
		}
	}
	else
	{
		tracker = old_tracker;
		subTreeP = tokenObjectPs[tracker]->process_primary_exp();
	}
	return subTreeP;
}

Token* int_postfix_exp()
{
	int old_tracker = tracker;

	auto subTreeP = tokenObjectPs[tracker]->process_int_id();
	if (subTreeP) {
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->process_postfix_operator()))
		{
				tObjP->add_childP(subTreeP);
				subTreeP = tObjP;
		}
	}
	else
	{
		tracker = old_tracker;
		subTreeP = tokenObjectPs[tracker]->process_int_primary_exp();
	}
	return subTreeP;
}

Token* comp_exp()
{
	int old_tracker = tracker;

		Token* tObjP;

		if(!(tObjP = tokenObjectPs[tracker]->process_int_comp()))
		{
			tracker = old_tracker;
			tObjP = postfix_exp();
		}
		return tObjP;
}

Token* int_comp_exp()
{
	int old_tracker = tracker;

		Token* tObjP;

		if(!(tObjP = tokenObjectPs[tracker]->process_int_comp()))
		{
			tracker = old_tracker;
			tObjP = int_postfix_exp();
		}
		return tObjP;
}

Token* int_div_exp()
{
	Token* ueTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = int_comp_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()) ||
				(upperNodeP = tokenObjectPs[tracker]->advance_past_mod_operator()))
		{
			if(!(ueTreeP = int_comp_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(ueTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* div_exp()
{
	int old_tracker = tracker;

	Token* ueTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = comp_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()))
		{
			if(!(ueTreeP = comp_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(ueTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	else
	{
		tracker = old_tracker;
		lowerNodeP = int_div_exp();
	}

	return lowerNodeP;
}

Token* additive_exp()
{
	Token* deTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	if ((lowerNodeP = div_exp()))
	{
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_additive_operator()))
		{
			deTreeP = div_exp();
			if(!deTreeP)
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(deTreeP);
			lowerNodeP = upperNodeP;
		}
	}

	return lowerNodeP;
}

Token* int_additive_exp()
{
	Token* deTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = int_div_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_additive_operator()))
		{
			if(!(deTreeP = int_div_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(deTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* shift_exp()
{
	Token* subTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	if ((lowerNodeP = int_additive_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_shift_operator()))
		{
			if(!(subTreeP = int_additive_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(subTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* ternary_exp()
{
	int old_tracker = tracker;
	Token* subTreeP;

	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->advance_past_conditional_operator()))
		{
			tObjP->add_childP(subTreeP);
			subTreeP = assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					auto colon_tracker = tracker - 1;
					subTreeP = ternary_exp();
					if(subTreeP)
					{
						delete tokenObjectPs[colon_tracker];
						tokenObjectPs[colon_tracker] = nullptr;
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}

	tracker = old_tracker;
	subTreeP = shift_exp();

	if(!subTreeP)
	{
		tracker = old_tracker;
		subTreeP = additive_exp();
	}
	return subTreeP;
}

Token* int_ternary_exp()
{
	int old_tracker = tracker;
	Token* subTreeP;

	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->advance_past_conditional_operator()))
		{
			tObjP->add_childP(subTreeP);
			subTreeP = int_assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					subTreeP = int_ternary_exp();
					if(subTreeP)
					{
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}
	tracker = old_tracker;
	return shift_exp();
}

Token* assignment_exp()
{
	int old_tracker = tracker;

	// int_id int_assignment_operator int_assignment_exp
	Token* subTreeP = tokenObjectPs[tracker]->process_int_id();
	if(subTreeP)
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_int_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	// production: id assignment_operator assignment_exp
	tracker = old_tracker;
	if ((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_gen_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				// we have assignment operator, but not valid assignment expression,
				//	so there must be an error and it should be reported.
				return nullptr;
		}
		// no assignment operator, so this is not this production

		// start production:  ternary_exp
		if(tokenObjectPs[tracker]->has_string_value(";"))
			// we have already processed an id, which satisfies the ternary_exp production.
			return subTreeP;
	}

	// production:  ternary_exp
	tracker = old_tracker;
	return ternary_exp();
}

Token* int_assignment_exp()
{
	int old_tracker = tracker;

	Token* subTreeP = tokenObjectPs[tracker]->process_int_id();
	if(subTreeP)
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_gen_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}

		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_int_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	// production:  int_ternary_exp
	tracker = old_tracker;
	return int_ternary_exp();
}

Token* stat()
{
	if(auto subTreeP = tokenObjectPs[tracker]->process_declaration())
		return subTreeP;
	else
	{
		return assignment_exp();
	}
}

// because the below function deals with ";", it is a bit different than the pattern seen elsewhere.
Token* stat_list()
{
	Token* statTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = stat())) {

		while (tokenObjectPs[tracker]->has_string_value(";")) {
			tracker++;
			if (tracker == tokenObjectPs.size()) {
				delete tokenObjectPs[tracker - 1];
				tokenObjectPs[tracker - 1] = nullptr;
				return lowerNodeP;
			}
			upperNodeP = tokenObjectPs[tracker - 1];

			if(!(statTreeP = stat()))
				throw ensc251::TokenException(tokenObjectPs[tracker]);

			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(statTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	throw ensc251::TokenException(tokenObjectPs[tracker]);
}

Token* recursive_parser()
{
	if (tokenObjectPs.size())
		return stat_list();
	else
		throw ensc251::TokenException();
}

shared_ptr_to_constant create(float v) {
		return shared_ptr_to_constant(new numeric_const<float>(v));
}

shared_ptr_to_constant
type_spec::
eval()
{
	evalMap[this->getChildToken(0)->getStringValue()].sptc = nullptr; // storing value for id as nullptr in map
	evalMap[this->getChildToken(0)->getStringValue()].ts = *this;
	return shared_ptr_to_constant();
};

} // namespace ensc251

