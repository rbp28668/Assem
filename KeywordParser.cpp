// KeywordParser.cpp: implementation of the KeywordParser class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "KeywordParser.hpp"
#include "AssemblyUnit.hpp"
#include "Expression.hpp"
#include "Exception.hpp"
#include "InputLine.hpp"
#include "SymbolTable.hpp"
#include "Plant.hpp"
#include "Segment.hpp"
#include "Module.h"
#include "Error.h"

KEYMATCH KeywordParser::keystore[]={
			"public",key_public,
			"external",key_external,
			"defb",key_defb,
			"defw",key_defw,
			"defm",key_defm,
			"defs",key_defs,
			"org",key_org,
			"include",key_include,
			"*include",key_include,		/*ZASM compatability */
			"cond",key_cond,
			"else",key_else,
			"endc",key_endc,
			"macro",key_macro,
			"endmac",key_endmac,
			"module",key_module,
			"endmod",key_endmod,
			"equ",key_equ,
			"reloc",key_reloc,
			"code",key_code,
			"data",key_data,
			"phase",key_phase,
			"radix",key_radix,
			"radb",key_radb,
			"rado",key_rado,
			"radd",key_radd,
			"radh",key_radh,
			"comment",key_comment,
			NULL,key_KEYMAX};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KeywordParser::KeywordParser(InputLine* input, AssemblyUnit* au, Plant* pl)
: line(input)
, unit(au)
, plant(pl)
, inCond(false)
{
	assert(input);
	assert(au);
	assert(pl);
}

KeywordParser::~KeywordParser()
{

}

void KeywordParser::doPublic(int pass)
{
	char* label = line->get_name();
	if(label == 0 && pass == 1)
		Exception::Exception::error(NO_PUBLIC_SYM, line);

	/* algorithm for this bit......
	get name
	if pass !=1
		if(name in global st)
			set scope public;
		else
			error UNRESOLVED SYMBOL
	*/

	// Do nothing first pass as no guarantee symbol will be in the symbol 
	// table at this time.
	if (pass == 2)
	{
		Symbol* symbol = unit->getGlobals().find(label);
		if(symbol != 0)/* if in gst */
			{
			symbol->setScope(Symbol::_public);			/* make public */
			}
		else
			{
			Exception::Exception::error(UNRESOLVED_SYMBOL, line);
			}
	}
	delete [] label;
}

void KeywordParser::doExternal(int pass)
{
	char* label = line->get_name();
	if(pass == 1)
	{
		if(unit->getGlobals().contains(label))
			Exception::Exception::error(EXT_OWN_CLASH, line);

		Symbol* symbol = new Symbol();
		symbol->setName(label);
		symbol->setSegment(0);
		symbol->setAddress(0);
		symbol->setScope(Symbol::external);
		symbol->setResolved(true);	/* not so but this is the linker's problem */

		unit->getGlobals().insert(symbol);

	}
	delete [] label;
}

void KeywordParser::doDefb(int pass)
{
	Expression* expr = line->get_expression();
	int value = 0;
	if(expr->resolve_expr(value))
	{
		if(((value > 255)||(value < -128))&&(pass==1))
			Exception::error(DEFB_RANGE, line);
		plant->plant_byte(value);
	}
}

void KeywordParser::doDefw(int pass)
{
	plant->plant_expr_word(line->get_expression());
}

void KeywordParser::doDefm(int pass)
{
	char* string = line->get_string();
	plant->plant_string(string);
}


void KeywordParser::doDefs(int pass)
{
	Expression* expr = line->get_expression();
	int value = 0;
	expr->resolve_expr(value);
	plant->plant_defs(value);
}
			
void KeywordParser::doOrg(int pass)
{
	Expression* expr = line->get_expression();
	int posn = 0;
	expr->resolve_expr(posn);
	plant->getCurrentSegment().setAddress(posn);
	plant->plant_org(posn);
	unit->setRelocatable(false);
}

void KeywordParser::doInclude(int pass)
{
	char* fn = line->get_filename();
	
	AssemblyUnit newUnit(unit);
	newUnit.process(fn);
}

void KeywordParser::doCond(int pass)
{
	Expression* expr = line->get_expression();
	int value = 0;
	expr->resolve_expr(value);
	
	inCond = (value!=0);		/* treat exp as boolean */
	if (!inCond)				/* if false skip to else */
		while(line->read_line())
			{
			line->strip_comments(';');
			line->skipLabel();
			if(line->blank_line()) continue;
			if(line->get_keyword(keystore) == key_else) return; 
			}
}

void KeywordParser::doElse(int pass)
{
	/* if last cond was true need to skip to endc */
	if(inCond)
		{
		while(line->read_line())
			{
			line->strip_comments(';');
			line->skipLabel();
			if(line->blank_line()) continue;
			if(line->get_keyword(keystore) == key_endc) return;
			}
		}
}

void KeywordParser::doEndc(int pass)
{
	/* cond doesn't do anything- just acts as marker for else */
}


void KeywordParser::doMacro(int pass)
{
	if(pass==1) Exception::error(MAC_NOT_IMP, line);
}

void KeywordParser::doEndmac(int pass)
{
	if(pass==1) Exception::error(MAC_NOT_IMP,line);
	return;
}

void KeywordParser::doModule(int pass)
{
	if((unit->getScope() == Symbol::local)&&(pass==1))
		Exception::error(NESTED_MODULES, line);
		
	char* name = line->get_name();

	if(pass==1)
		{
		if(unit->getModules().contains(name))
			Exception::error(MULT_DEF_MODULE, line);
		else
			{
			Module* module = new Module();
			unit->getModules().addModule(name,module);
			}

		if(unit->getGlobals().contains(name))
			Exception::error(MOD_GLOBAL_CLASH, line);
		else
			{
			// Start of a module is it's entry point so it's put in
			// the global symbol table.
			Symbol* symbol = new Symbol();
			symbol->setName(name);
			symbol->setScope(Symbol::_private);
			symbol->setSegment(&(plant->getCurrentSegment()));
			symbol->setAddress(plant->getCurrentSegment().getAddress());
			symbol->setResolved(true);
			unit->getGlobals().insert(symbol);
			}
		}
	else				/* pass==2 */
		{
		Module* module = unit->getModules().lookup(name);
				
		SymbolTable& local = module->getSymbols();
		unit->linkSymbols(&local);
		}
	
	unit->setScope(Symbol::local);

	delete [] name;
}

void KeywordParser::doEndmod(int pass)
{
	if((unit->getScope() != Symbol::local)&&(pass==1))
		Exception::error(ENDMOD_NOT_IN_MODULE, line);
	
	if(pass==1)
		unit->unlinkSymbols();

	unit->setScope(Symbol::_private);				/* back to global */
}


void KeywordParser::doEqu(int pass)
{
	if (pass==1)
		Exception::error(ILLEGAL_EQU, line);		/* handled as special case */
}

void KeywordParser::doReloc(int pass)
{
	unit->setRelocatable(true);
	plant->plant_reloc();
}

void KeywordParser::doCode(int pass)
{
	char* name = "code";
	plant->plant_segment(name);
}

void KeywordParser::doData(int pass)
{
	char* name = "data";
	plant->plant_segment(name);
}

void KeywordParser::doSegment(int pass)
{
	char* name = line->get_name();
	plant->plant_segment(name);
}

void KeywordParser::doPhase(int pass)
{
	Expression* expr = line->get_expression();
	int bdy = 256;
	expr->resolve_expr(bdy);
	if (unit->isRelocatable())
		plant->plant_phase(bdy);
	else
		{
		Segment& seg = plant->getCurrentSegment();
		int addr = (1 + seg.getAddress()/bdy) * bdy;
		plant->plant_org(addr);
	}

}

void KeywordParser::doRadix(int pass)
{
	Expression* expr = line->get_expression();
	int radix = 10;
	expr->resolve_expr(radix);
	unit->setRadix(radix);
}

void KeywordParser::doRadb(int pass)
{
	unit->setRadix(2);
}

void KeywordParser::doRado(int pass)
{
	unit->setRadix(8);
}

void KeywordParser::doRadd(int pass)
{
	unit->setRadix(10);
}

void KeywordParser::doRadh(int pass)
{
	unit->setRadix(16);
}	

void KeywordParser::doComment(int pass)
{
	char* string = line->get_string();
	plant->plant_comment(string);
	delete [] string;
}


void KeywordParser::execute_keyword(int key, int pass)
{

switch (key)
 	{

	/****** PUBLIC ******/
	case key_public:
		doPublic(pass);
		break;
	
	/****** EXTERNAL *******/
	case key_external:
		doExternal(pass);
		break;
	
	/****** DEFB ******/
	case key_defb:
		doDefb(pass);
		break;

	/****** DEFW ******/
	case key_defw:
		doDefw(pass);
		break;

	/****** DEFM ******/
	case key_defm:
		doDefm(pass);
		break;

	/****** DEFS ******/
	case key_defs:
		doDefs(pass);
		break;
			
	/****** ORG ******/
	case key_org:
		doOrg(pass);
		break;

	/****** INCLUDE ******/
	case key_include:
		doInclude(pass);
		break;

	/****** COND ******/
	case key_cond:
		doCond(pass);
		break;

	/******* ELSE ******/
	case key_else:
		doElse(pass);
		break;

	/******* ENDC *******/
	case key_endc:
		doEndc(pass);
		break;

	/******* MACRO ******/
	case key_macro:
		doMacro(pass);
		break;

	/******* ENDMAC ******/
	case key_endmac:
		doEndmac(pass);
		break;

	/******* MODULE ******/
	case key_module:
		doModule(pass);
		break;

	/******* ENDMOD *******/
	case key_endmod:
		doEndmod(pass);
		break;

	/****** EQU *******/
	case key_equ:
		doEqu(pass);
		break;

	/****** RELOC ******/
	case key_reloc:
		doReloc(pass);
		break;

	/******* CODE *******/
	case key_code:
		doCode(pass);
		break;

	/******* DATA *******/
	case key_data:
		doData(pass);
		break;

	/******* PHASE *******/
	case key_phase:
		doPhase(pass);
		break;

	/****** RADIX ******/
	case key_radix:
		doRadix(pass);
		break;

	/******* RADB *******/
	case key_radb:
		doRadb(pass);
		break;

	/******* RADO *******/
	case key_rado:
		doRado(pass);
		break;

	/******* RADD *******/
	case key_radd:
		doRadd(pass);
		break;

	/******* RADH *******/
	case key_radh:
		doRadh(pass);
		break;
	
	/****** COMMENT ******/
	case key_comment:
		doComment(pass);
		break;

	default:
		Exception::fatal("Illegal keyword code");
		break;
	} /*endswitch */
return;
}

bool KeywordParser::parse(int pass)
{
	int key = line->get_keyword(keystore);
	if( key == -1)
		return false;
	else
		execute_keyword(key, pass);
	return true;
}

/****************************************************************/
/** PROCESS_EQU												**/
/** Parameters:												**/
/** Returns: 													**/
/****************************************************************/
bool KeywordParser::parseEqu(int pass)
{
	/** note that equ is processed differently from everything else as it is the
	only type of statement that starts without a keyword or assembler mnemonic */

	line->save_buffer();			/* use a copy of the input for parsing */

	char* name = line->get_name();
	if(name == 0) 
		{
		line->free_buffer();
		return false;
		}

	if(line->get_keyword(keystore) != key_equ) 
		{
		line->free_buffer();
		return false;
		}

	Expression* ex = line->get_expression();

	if(pass==2)			/* only suck in text on pass 2 as symbol */
		{					/* table will have been set up on pass 1 */
		delete [] name;
		line->free_buffer();
		delete ex;
		return true;  
		}

	assert(pass == 1);

	/* Ok so on pass 1 try to resolve the expression and stuff the */
	/* value into the symbol table. Note that equates must be resolvable*/
	/* on pass 1 */

	int value = 0;
	if(!ex->resolve_expr(value))
		{
		delete [] name;
		line->free_buffer();
		return false;
		}	

	
	if(unit->getGlobals().contains(name))
		{
		Exception::error(MULT_GLOBAL_DEF, line);
		line->free_buffer();
		delete [] name;
		return true;
		}

	Symbol* symbol = new Symbol();
	symbol->setName(name);
	symbol->setSegment(&(plant->getCurrentSegment()));
	symbol->setAddress(plant->getCurrentSegment().getAddress());
	symbol->setScope(Symbol::equate);
	symbol->setResolved(true);

	unit->getGlobals().insert(symbol);

	line->free_buffer();
	delete [] name;
	return true;
}

