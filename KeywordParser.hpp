// KeywordParser.hpp: interface for the KeywordParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef KEYWORDPARSER_HPP_INCLUDED
#define KEYWORDPARSER_HPP_INCLUDED


class AssemblyUnit;
class InputLine;
class Plant;

struct KEYMATCH;

class KeywordParser  
{

	enum kwds {
		key_duff=-1, 
		key_public,
		key_external,
		key_defb,
		key_defw,
		key_defm,
		key_defs,
		key_org,
		key_include,
		key_cond,
		key_else,
		key_endc,
		key_macro,
		key_endmac,
		key_module,
		key_endmod,
		key_equ,
		key_reloc,
		key_code,
		key_data,
		key_phase,
		key_radix,
		key_radb,
		key_rado,
		key_radd,
		key_radh,
		key_comment,
		key_KEYMAX
		};
typedef enum kwds KEYWORD;

	InputLine* line; 
	static KEYMATCH keystore[];
	AssemblyUnit* unit;
	Plant* plant;
	bool inCond; // true if processing conditional.

	void doPublic(int pass);
	void doExternal(int pass);
	void doDefb(int pass);
	void doDefw(int pass);
	void doDefm(int pass);
	void doDefs(int pass);
	void doOrg(int pass);
	void doInclude(int pass);
	void doCond(int pass);
	void doElse(int pass);
	void doEndc(int pass);
	void doMacro(int pass);
	void doEndmac(int pass);
	void doModule(int pass);
	void doEndmod(int pass);
	void doEqu(int pass);
	void doReloc(int pass);
	void doCode(int pass);
	void doData(int pass);
	void doSegment(int pass);
	void doPhase(int pass);
	void doRadix(int pass);
	void doRadb(int pass);
	void doRado(int pass);
	void doRadd(int pass);
	void doRadh(int pass);
	void doComment(int pass);

	void execute_keyword(int key, int pass);
public:
	KeywordParser(InputLine* input, AssemblyUnit* unit, Plant* pl);
	virtual ~KeywordParser();

	bool parse(int pass);
	bool parseEqu(int pass);
};

#endif 
