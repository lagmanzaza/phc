/*
 * phc -- the open source PHP compiler
 * See doc/license/README.license for licensing information
 *
 * Check for any assignments from temporary variables to other temporary
 * variables. All of these are superfluous, and indicates poor shredding.
 */

#include "process_ast/AST_unparser.h"
#include "process_hir/HIR_unparser.h"
#include "process_mir/MIR_unparser.h"
#include "pass_manager/Plugin_pass.h"
#include "process_ir/General.h"

static bool success = true;

extern "C" void load (Pass_manager* pm, Plugin_pass* pass)
{
	pm->add_after_each_pass (pass);
}

template <
	class Visitor,
	class Variable,
	class Assignment
>
class Check_assignment : public Visitor
{
	void pre_assignment(Assignment* in)
	{
		if (in->variable->variable_name->attrs->is_true ("phc.codegen.st_entry_not_required"))
		{
			Variable* var = dynamic_cast<Variable*> (in->expr);
			if (var && var->variable_name->attrs->is_true ("phc.codegen.st_entry_not_required"))
			{
				debug (in);
				xadebug (in);
				success = false;
			}
		}
	}
};

extern "C" void run_ast (AST::PHP_script* in, Pass_manager*) 
{
	in->visit (new Check_assignment <AST::Visitor, AST::Variable, AST::Assignment> );
}

extern "C" void run_hir (HIR::PHP_script* in, Pass_manager*) 
{
	in->visit (new Check_assignment <HIR::Visitor, HIR::Variable, HIR::Assignment> );
}

extern "C" void run_mir (MIR::PHP_script* in, Pass_manager*)
{ 
	in->visit (new Check_assignment <MIR::Visitor, MIR::Variable, MIR::Assignment> );
}

extern "C" void unload ()
{
	if (success)
		printf("Success\n");
	else
		printf("Failure\n");
}