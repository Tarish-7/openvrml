#ifndef INC_RecognitionException_hpp__
#define INC_RecognitionException_hpp__

/**
 * <b>SOFTWARE RIGHTS</b>
 * <p>
 * ANTLR 2.6.0 MageLang Insitute, 1999
 * <p>
 * We reserve no legal rights to the ANTLR--it is fully in the
 * public domain. An individual or company may do whatever
 * they wish with source code distributed with ANTLR or the
 * code generated by ANTLR, including the incorporation of
 * ANTLR, or its output, into commerical software.
 * <p>
 * We encourage users to develop software with ANTLR. However,
 * we do ask that credit is given to us for developing
 * ANTLR. By "credit", we mean that if you use ANTLR or
 * incorporate any source code into one of your programs
 * (commercial product, research project, or otherwise) that
 * you acknowledge this fact somewhere in the documentation,
 * research report, etc... If you like ANTLR and have
 * developed a nice tool with the output, please mention that
 * you developed it using ANTLR. In addition, we ask that the
 * headers remain intact in our source code. As long as these
 * guidelines are kept, we expect to continue enhancing this
 * system and expect to make other tools available as they are
 * completed.
 * <p>
 * The ANTLR gang:
 * @version ANTLR 2.6.0 MageLang Insitute, 1999
 * @author Terence Parr, <a href=http://www.MageLang.com>MageLang Institute</a>
 * @author <br>John Lilley, <a href=http://www.Empathy.com>Empathy Software</a>
 * @author <br><a href="mailto:pete@yamuna.demon.co.uk">Pete Wells</a>
 */

#include "config.hpp"
#include "ANTLRException.hpp"

ANTLR_BEGIN_NAMESPACE(antlr)

class RecognitionException : public ANTLRException {
public:
	ANTLR_USE_NAMESPACE(std)string fileName; // not used by treeparsers
	int line;    // not used by treeparsers
	int column;  // not used by treeparsers

	RecognitionException();

	RecognitionException(const ANTLR_USE_NAMESPACE(std)string& s);

	RecognitionException(const ANTLR_USE_NAMESPACE(std)string& s,const ANTLR_USE_NAMESPACE(std)string& fileName_,int line);
	RecognitionException(const ANTLR_USE_NAMESPACE(std)string& s,const ANTLR_USE_NAMESPACE(std)string& fileName_,int line,int column);
	~RecognitionException() throw() {}

	/**
	 * @return the column number that this exception happened on.
	 * @author Shawn P. Vincent (svincent@svincent.com)
	 */
	virtual int getColumn() const;
	/**
	 * @deprecated As of ANTLR 2.7.0
	 */
	virtual ANTLR_USE_NAMESPACE(std)string getErrorMessage() const;
protected:
	virtual ANTLR_USE_NAMESPACE(std)string getFileLineString() const;
public:
	virtual ANTLR_USE_NAMESPACE(std)string getFilename() const;
	/**
	 * @return the line number that this exception happened on.
	 * @author Shawn P. Vincent (svincent@svincent.com)
	 */
	virtual int getLine() const;
	virtual ANTLR_USE_NAMESPACE(std)string toString() const;
};

ANTLR_END_NAMESPACE

#endif //INC_RecognitionException_hpp__