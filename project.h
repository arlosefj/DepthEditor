#ifndef _PROJECT_H_

#define _PROJECT_H_

#define FILE_NAME_SIZE  256

//! all input parameters
typedef struct
{
	char projectname[128];
	char inputfile[FILE_NAME_SIZE];
	char infofile[FILE_NAME_SIZE];
	char outputfile1[FILE_NAME_SIZE];
	char outputfile2[FILE_NAME_SIZE];
	//char h264encparam[FILE_NAME_SIZE];

	int inputtype;
	int inputwidth;
	int inputheight;
	int outputtype;

	int videoinfoflag;
	int keyframeflag;
	
	int keyframeth;

	int timestart;
	int timeend;
	
	int sectionflag;
	int sectionduration;
	int sectioncur;
	int sectionnum;
	
	int segparanor;
	int segparawsd;
	int segparanoi;
	int segparaemm;
	int segparaemr;

	int segparaaet;
	int segparaeei;
	int segparaees;
	int segparaeet;

	int infparaim;
	int infparais;
	
	int depparafilt;
	int depparadfi;
	int depparadfgsx;
	int depparadfgsy;
	int depparafem;
	int depparaeem;
	int depparaemr;
	int depparaari;

	int renparaof;
	int renpararm;
	int renpararpp;
	int renparaalp;
	int renparafp;
	int renparaesm;
	int renparaesth;
	int renparafem;
	int renparafer;
	int renparaferr;
	int renparafemm;
	int renparafesp;
	
}InputParameters;

//! Maps parameter name to its address, type etc.
typedef struct {
  char *TokenName;    //!< name
  void *Place;        //!< address
  int Type;           //!< type:  0-int, 1-char[], 2-double
  double Default;     //!< default value
  int param_limits;   //!< 0: no limits, 1: both min and max, 2: only min (i.e. no negatives), 3: special case 
  double min_limit;
  double max_limit;
  int    char_size;   //!< Dimension of type char[]
} Mapping;

extern InputParameters cfgparams;

extern Mapping ParaMap[];


#endif /*_PROJECT_H_*/

