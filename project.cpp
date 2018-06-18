#include "common.h"
#include "project.h"
#include <io.h>

#define MAX_ITEMS_TO_PARSE  300

char g_projectpath[FILE_NAME_SIZE];

FILE * g_fileproject = NULL;
int g_isprojectopen = 0;

GtkWidget *g_wdgProjectOpenSelection;
GtkWidget *g_wdgCreateNewProject;

GtkWidget *g_wdgInputfileOpenSelection;
GtkWidget *g_wdgInfofileOpenSelection;
GtkWidget *g_wdgOutputfileOpenSelection;

InputParameters cfgparams;

// Mapping_Map Syntax:
// {NAMEinConfigFile,  &cfgparams.VariableName, Type, InitialValue, LimitType, MinLimit, MaxLimit, CharSize}
// Types : {0:int, 1:text, 2: double}
// LimitType: {0:none, 1:both, 2:minimum, 3: Special}
// We could separate this based on types to make it more flexible and allow also defaults for text types.
Mapping ParaMap[] = {
	/*TokenName*/			/*Place*/					/*Type*/	/*Default*/	/*Param_limits*/	/*min_limit*/	/*max_limit*/	/*char_size*/
    {"ProjectName",			&cfgparams.projectname,		1,			0.0,		0,				0.0,		0.0,			FILE_NAME_SIZE, },
/*Files*/
    {"InputFileName",		&cfgparams.inputfile,		1,			0.0,		0,				0.0,		0.0,			FILE_NAME_SIZE, },
    {"InfoFileName",		&cfgparams.infofile,		1,			0.0,		0,				0.0,		0.0,			FILE_NAME_SIZE, },
    {"OutputFileName1",		&cfgparams.outputfile1,		1,			0.0,		0,				0.0,		0.0,			FILE_NAME_SIZE, },
	{"OutputFileName2",		&cfgparams.outputfile2,		1,			0.0,		0,				0.0,		0.0,			FILE_NAME_SIZE, },
	//{"H264EncParameter", 	&cfgparams.h264encparam,	1,			0.0,		0,				0.0,		0.0,			FILE_NAME_SIZE, },
/*Input Output Parameters*/
    {"InputType",			&cfgparams.inputtype,		0,   		0.0,		1,      		0.0,  		3.0,			},
    {"InputWidth",			&cfgparams.inputwidth,		0,   		480.0,		1,      		4.0,		1920.0,			},
    {"InputHeight",			&cfgparams.inputheight,		0,   		270.0,		1,      		4.0,		1080.0,			},
    {"OutputType",			&cfgparams.outputtype,		0,   		0.0,		1,      		0.0,  		3.0,			},
/*Video Start and End Time*/
    {"VideoInfo",			&cfgparams.videoinfoflag,	0,   		0.0,		1,      		0.0,  		1.0,			},
    {"TimeStart",			&cfgparams.timestart,		0,   		0.0,		1,      		0.0,  		100000.0,		},
	{"TimeEnd",				&cfgparams.timeend,			0,			0.0,		1,				0.0,		100000.0,		},
	{"KeyFrame",			&cfgparams.keyframeflag,	0,			0.0,		1,				0.0,		1.0,			},
	{"KeyFrameTH", 			&cfgparams.keyframeth, 		0,			512.0,		1,				0.0,		1000.0, 		},
/*Video Setions*/
    {"SectionFlag",			&cfgparams.sectionflag,		0,   		0.0,		1,      		0.0,  		1.0,			},
	{"SectionDuration",		&cfgparams.sectionduration,	0,			30.0,		1,				0.0,		1000.0,			},
	{"SectionNum",			&cfgparams.sectionnum,		0,			0.0,		1,				0.0,		100000.0,		},
	{"SectionCur",			&cfgparams.sectioncur,		0,			0.0,		1,				0.0,		100000.0,		},
/*Segment Parameters*/
	{"SegParaNoR",			&cfgparams.segparanor,		0,   		180.0,		1,      		2.0,  		500.0,			},
	{"SegParaWSD",			&cfgparams.segparawsd,		0,   		10.0,		1,      		5.0,  		20.0,			},
	{"SegParaNoI",			&cfgparams.segparanoi,		0,   		10.0,		1,      		2.0,  		20.0,			},
	{"SegParaEMM",			&cfgparams.segparaemm,		0,   		0.0,		1,      		0.0,  		1.0,			},
	{"SegParaEMR",			&cfgparams.segparaemr,		0,			2.0,		1,				1.0,		30.0,			},
	{"SegParaAET",			&cfgparams.segparaaet,		0,			6.0,		1,				1.0,		30.0,			},
	{"SegParaEEI",			&cfgparams.segparaeei,		0,			5.0,		1,				1.0,		10.0,			},
	{"SegParaEES",			&cfgparams.segparaees,		0,			10.0,		1,				5.0,		30.0,			},
	{"SegParaEET",			&cfgparams.segparaeet,		0,			6.0,		1,				1.0,		30.0,			},
/*Inference Parameters*/
	{"InfParaIM",			&cfgparams.infparaim,		0,   		0.0,		1,      		0.0,  		1.0,			},
	{"InfParaIS",			&cfgparams.infparais,		0,			3.0,		1,				1.0,		6.0,			},
/*Depth Parameters*/
	{"DepParaFILT",			&cfgparams.depparafilt,		0,   		0.0,		0,      		0.0,  		0.0,			},
	{"DepParaDFI",			&cfgparams.depparadfi,		0,			3.0,		1,				0.0,		5.0,			},
	{"DepParaDFGSX",		&cfgparams.depparadfgsx,	0,			30.0,		1,				1.0,		50.0,			},
	{"DepParaDFGSY",		&cfgparams.depparadfgsy,	0,			30.0,		1,				1.0,		50.0,			},
	{"DepParaFEM",			&cfgparams.depparafem,		0,   		1.0,		1,      		0.0,  		3.0,			},
	{"DepParaEEM",			&cfgparams.depparaeem,		0,   		1.0,		1,      		0.0,  		1.0,			},
	{"DepParaEMR",			&cfgparams.depparaemr,		0,   		10.0,		1,      		1.0,  		30.0,			},
	{"DepParaARI",			&cfgparams.depparaari,		0,   		32.0,		1,      		4.0,  		64.0,			},
/*Render Parameters*/
	{"RenParaOF",			&cfgparams.renparaof,		0,   		0.0,		1,      		0.0,  		1.0,			},
	{"RenParaRM",			&cfgparams.renpararm,		0,   		2.0,		1,      		0.0,  		2.0,			},
	{"RenParaRPP",			&cfgparams.renpararpp,		0,   		0.0,		1,      		0.0,  		3.0,			},
	{"RenParaALP",			&cfgparams.renparaalp,		0,   		10.0,		1,      		1.0,  		50.0,			},
	{"RenParaFP",			&cfgparams.renparafp,		0,			127.0,		1,				0.0,		255.0,			},
	{"RenParaESM",			&cfgparams.renparaesm,		0,			1.0,		1,				0.0,		2.0,			},
	{"RenParaESTH",			&cfgparams.renparaesth,		0,			10.0,		1,				0.0,		255.0,			},
	{"RenParaFEM",			&cfgparams.renparafem,		0,   		0.0,		1,      		0.0,  		1.0,			},
	{"RenParaFER",			&cfgparams.renparafer,		0,   		5.0,		1,      		1.0,  		15.0,			},
	{"RenParaFERR",			&cfgparams.renparaferr,		0,   		30.0,		1,      		10.0,  		60.0,			},
	{"RenParaFEMM",			&cfgparams.renparafemm,		0,   		1.0,		1,      		0.0,  		2.0,			},
	{"RenParaFESP",			&cfgparams.renparafesp,		0,   		1.0,		1,      		0.0,  		1.0,			},

/*NULL*/
    {NULL,					NULL,						-1,			0.0,		0,				0.0,		0.0,			},
    
};

/*!
 ***********************************************************************
 * \brief
 *    allocates memory buf, opens file Filename in f, reads contents into
 *    buf and returns buf
 * \param Filename
 *    name of config file
 * \return
 *    if successfull, content of config file
 *    NULL in case of error. Error message will be set in errortext
 ***********************************************************************
 */
char *GetProjectFileContent (char *Filename)
{
  long FileSize;
  FILE *f;
  char *buf;

  if (fopen_s(&f, Filename, "r")!=0)
  {
      printf("PROJECT ERROR --- Cannot open Project file %s.", Filename);
      return NULL;
  }

  if (0 != fseek (f, 0, SEEK_END))
  {
    printf("PROJECT ERROR --- Cannot fseek in Project file %s.", Filename);
    return NULL;
  }

  FileSize = ftell (f);
  if (FileSize < 0 || FileSize > 100000)
  {
    printf("PROJECT ERROR --- Unreasonable Filesize %ld reported by ftell for Project file %s.", FileSize, Filename);
    return NULL;
  }
  if (0 != fseek (f, 0, SEEK_SET))
  {
    printf("PROJECT ERROR --- Cannot fseek in Project file %s.", Filename);
    return NULL;
  }

  if ((buf = (char *)malloc (FileSize + 1))==NULL)
  {
  	printf("PROJECT ERROR --- Cannot malloc filesize buffer.");
	return NULL;
  }

  // Note that ftell() gives us the file size as the file system sees it.  The actual file size,
  // as reported by fread() below will be often smaller due to CR/LF to CR conversion and/or
  // control characters after the dos EOF marker in the file.

  FileSize = (long) fread (buf, 1, FileSize, f);
  buf[FileSize] = '\0';


  fclose (f);
  return buf;
}

/*!
 ***********************************************************************
 * \brief
 *    Returns the index number from Map[] for a given parameter name.
 * \param Map
 *    Mapping structure
 * \param s
 *    parameter name string
 * \return
 *    the index number if the string is a valid parameter name,         \n
 *    -1 for error
 ***********************************************************************
 */
static int ParameterNameToMapIndex (Mapping *Map, char *s)
{
  int i = 0;

  while (Map[i].TokenName != NULL)
    if (0==_strcmpi(Map[i].TokenName, s))
      return i;
    else
      i++;
  return -1;
}

/*!
 ***********************************************************************
 * \brief
 *    Parses the character array buf and writes global variable input, which is defined in
 *    configfile.h.  This hack will continue to be necessary to facilitate the addition of
 *    new parameters through the Map[] mechanism (Need compiler-generated addresses in map[]).
 * \param p_Inp
 *    InputParameters of configuration
 * \param Map
 *    Mapping structure to specify the name and value mapping relation
 * \param buf
 *    buffer to be parsed
 * \param bufsize
 *    buffer size of buffer
 ***********************************************************************
 */
void ParseContent (InputParameters *p_Inp, Mapping *Map, char *buf, int bufsize)
{
  char *items[MAX_ITEMS_TO_PARSE] = {NULL};
  int MapIdx;
  int item = 0;
  int InString = 0, InItem = 0;
  char *p = buf;
  char *bufend = &buf[bufsize];
  int IntContent;
  double DoubleContent;
  int i;

  // Stage one: Generate an argc/argv-type list in items[], without comments and whitespace.
  // This is context insensitive and could be done most easily with lex(1).

  while (p < bufend)
  {
    switch (*p)
    {
    case 13:
      ++p;
      break;
    case '#':                 // Found comment
      *p = '\0';              // Replace '#' with '\0' in case of comment immediately following integer or string
      while (*p != '\n' && p < bufend)  // Skip till EOL or EOF, whichever comes first
        ++p;
      InString = 0;
      InItem = 0;
      break;
    case '\n':
      InItem = 0;
      InString = 0;
      *p++='\0';
      break;
    case ' ':
    case '\t':              // Skip whitespace, leave state unchanged
      if (InString)
        p++;
      else
      {                     // Terminate non-strings once whitespace is found
        *p++ = '\0';
        InItem = 0;
      }
      break;

    case '"':               // Begin/End of String
      *p++ = '\0';
      if (!InString)
      {
        items[item++] = p;
        InItem = ~InItem;
      }
      else
        InItem = 0;
      InString = ~InString; // Toggle
      break;

    default:
      if (!InItem)
      {
        items[item++] = p;
        InItem = ~InItem;
      }
      p++;
    }
  }

  item--;

  for (i=0; i<item; i+= 3)
  {
    if (0 > (MapIdx = ParameterNameToMapIndex (Map, items[i])))
    {
      //snprintf (errortext, ET_SIZE, " Parsing error in config file: Parameter Name '%s' not recognized.", items[i]);
      //error (errortext, 300);
      printf ("PROJECT ERROR --- Parsing in config file: Parameter Name '%s' not recognized.", items[i]);
      i -= 2 ;
      continue;
    }
    if (_strcmpi ("=", items[i+1]))
    {
      printf ("PROJECT ERROR --- Parsing in config file: '=' expected as the second token in each line.");
    }

    // Now interpret the Value, context sensitive...

    switch (Map[MapIdx].Type)
    {
    case 0:           // Numerical
      if (1 != sscanf_s (items[i+2], "%d", &IntContent))
      {
        printf ("PROJECT ERROR --- Parsing Expected numerical value for Parameter of %s, found '%s'.", items[i], items[i+2]);
      }
      * (int *) (Map[MapIdx].Place) = IntContent;
      //printf (".");
      break;
    case 1:
      if (items[i + 2] == NULL)
        memset((char *) Map[MapIdx].Place, 0, Map[MapIdx].char_size);
      else
        strncpy ((char *) Map[MapIdx].Place, items [i+2], Map[MapIdx].char_size);
      //printf (".");
      break;
    case 2:           // Numerical double
      if (1 != sscanf_s (items[i+2], "%.2lf", &DoubleContent))
      {
        printf ("PROJECT ERROR --- Parsing Expected numerical value for Parameter of %s, found '%s'.", items[i], items[i+2]);
      }
      * (double *) (Map[MapIdx].Place) = DoubleContent;
      //printf (".");
      break;
    default:
      printf ("PROJECT ERROR --- Unknown value type in the map definition of project.h");
    }
  }
  *p_Inp = cfgparams;
}

/*!
 ***********************************************************************
 * \brief
 *    Sets initial values for encoding parameters.
 * \return
 *    -1 for error
 ***********************************************************************
 */
int InitParams(Mapping *Map)
{
  int i = 0;

  while (Map[i].TokenName != NULL)
  {
    if (Map[i].Type == 0)
        * (int *) (Map[i].Place) = (int) Map[i].Default;
    else if (Map[i].Type == 2)
    * (double *) (Map[i].Place) = Map[i].Default;
      i++;
  }
  return -1;
}

/*!
 ***********************************************************************
 * \brief
 *    Outputs parameters.
 * \return
 *    -1 for error
 ***********************************************************************
 */
 /*
int DisplayParams(Mapping *Map)
{
  int i = 0;

  printf("******************************************************\n");
  while (Map[i].TokenName != NULL)
  {
    if (Map[i].Type == 0)
      printf("Parameter %s = %d\n",Map[i].TokenName,* (int *) (Map[i].Place));
    else if (Map[i].Type == 1)
      printf("Parameter %s = ""%s""\n",Map[i].TokenName,(char *)  (Map[i].Place));
    else if (Map[i].Type == 2)
      printf("Parameter %s = %.2f\n",Map[i].TokenName,* (double *) (Map[i].Place));
      i++;
  }
  printf("******************************************************\n");
  return i;
}
*/
void LoadParameters(InputParameters * inp)
{
	g_segparamnumregion = H3D_MEDIAN(inp->segparanor, SEGPARAM_NUMREGIONMIN, SEGPARAM_NUMREGIONMAX);
	g_segparamweightspadis = H3D_MEDIAN(inp->segparawsd, SEGPARAM_WEIGHTSPADISMIN, SEGPARAM_WEIGHTSPADISMAX);
	g_segparamiterate = H3D_MEDIAN(inp->segparanoi, SEGPARAM_ITERATEMIN, SEGPARAM_ITERATEMAX);

	g_segparamautoedgeth = H3D_MEDIAN(inp->segparaaet, SEGPARAM_AUTOEDGETHMIN, SEGPARAM_AUTOEDGETHMAX);
	g_segparamedgeenlargeiter = H3D_MEDIAN(inp->segparaeei, SEGPARAM_EDGEENLARGEITERMIN, SEGPARAM_EDGEENLARGEITERMAX);
	g_segparamedgeenlargesize = H3D_MEDIAN(inp->segparaees, SEGPARAM_EDGEENLARGESIZEMIN, SEGPARAM_EDGEENLARGESIZEMAX);
	g_segparamedgeenlargeth = H3D_MEDIAN(inp->segparaeet, SEGPARAM_EDGEENLARGETHMIN, SEGPARAM_EDGEENLARGETHMAX);

	g_segeditmodule = H3D_MEDIAN(inp->segparaemm, SEGEDITMODULESQU, SEGEDITMODULECIR);
	g_segeditmodulerad = H3D_MEDIAN(inp->segparaemr, SEGEDITPARAM_RADMIN, SEGEDITPARAM_RADMAX);

	g_inferencemode = H3D_MEDIAN(inp->infparaim, INFMODEPIX, INFMODEREG);
	g_inferencestrength = H3D_MEDIAN(inp->infparais, INFPARAM_STRENGTHMIN, INFPARAM_STRENGTHMAX);
	
	g_depthprocessparam = inp->depparafilt&0xFF;
	g_depfiltdilateiter = H3D_MEDIAN(inp->depparadfi,DEPFILTPARAM_DIMIN,DEPFILTPARAM_DIMAX);
	g_depfiltgausesigmax = H3D_MEDIAN(inp->depparadfgsx,DEPFILTPARAM_GSMIN,DEPFILTPARAM_GSMAX);
	g_depfiltgausesigmay = H3D_MEDIAN(inp->depparadfgsy,DEPFILTPARAM_GSMIN,DEPFILTPARAM_GSMAX);
	g_depeditmode = H3D_MEDIAN(inp->depparafem, DEPEDITMODEINCFLAT, DEPEDITMODEDECGRAD);
	g_depeditmodule = H3D_MEDIAN(inp->depparaeem, DEPEDITMODULESQU, DEPEDITMODULECIR);
	g_depeditmodulerad = H3D_MEDIAN(inp->depparaemr, DEPEDITPARAM_RADMIN, DEPEDITPARAM_RADMAX);
	g_depautorefineiter = H3D_MEDIAN(inp->depparaari,DEPAUTOPARAM_RIMIN,DEPAUTOPARAM_RIMAX);
	
	g_renderoutputformat = H3D_MEDIAN(inp->renparaof, RENDEROUTLR, RENDEROUTAN);
	g_rendermethod = H3D_MEDIAN(inp->renpararm, RENDERWARP, RENDERPUSH);
	g_renderprocmethod = H3D_MEDIAN(inp->renpararpp, RENDERPROCNONE, RENDERPROCHHF);
	g_renderalpha = H3D_MEDIAN(inp->renparaalp, RENPARAM_ALPHAMIN, RENPARAM_ALPHAMAX);
	g_renderforcalplane = H3D_MEDIAN(inp->renparafp, RENPARAM_FPMIN, RENPARAM_FPMAX);
	g_renderedgesoftenmethod = H3D_MEDIAN(inp->renparaesm, RENDEREDGESOFTENNON, RENDEREDGESOFTENMID);
	g_renderedgesoftenth = H3D_MEDIAN(inp->renparaesth, RENPARAM_ESTHMIN, RENPARAM_ESTHMAX);
	g_rendereditselmodule = H3D_MEDIAN(inp->renparafem, RENEDITSELMODESQU, RENEDITSELMODECIR);
	g_rendereditselmodulerad = H3D_MEDIAN(inp->renparafer, RENEDITPARAM_SRADMIN, RENEDITPARAM_SRADMAX);
	g_rendereditrefregionrad = H3D_MEDIAN(inp->renparaferr, RENEDITPARAM_RRADMIN, RENEDITPARAM_RRADMAX);
	g_rendereditmatchmodule = H3D_MEDIAN(inp->renparafemm, RENEDITMATCHMOD7X7, RENEDITMATCHMODBXB);
	g_rendereditstartpoint = H3D_MEDIAN(inp->renparafesp, RENEDITSELSPSTRU, RENEDITSELSPMENU);
	
	return ;
}

void WriteProjectFile(InputParameters * params)
{
	fprintf(g_fileproject, "ProjectName\t\t=\t%s\n",params->projectname);
	fprintf(g_fileproject, "InputFileName\t\t=\t%s\n",params->inputfile);
	fprintf(g_fileproject, "InfoFileName\t\t=\t%s\n",params->infofile);
	fprintf(g_fileproject, "OutputFileName1\t\t=\t%s\n",params->outputfile1);
	fprintf(g_fileproject, "OutputFileName2\t\t=\t%s\n",params->outputfile2);
	//fprintf(g_fileproject, "H264EncParameter\t\t=\t%s\n",params->h264encparam);

	fprintf(g_fileproject, "InputType\t\t=\t%d\n",params->inputtype);
	fprintf(g_fileproject, "InputWidth\t\t=\t%d\n",params->inputwidth);
	fprintf(g_fileproject, "InputHeight\t\t=\t%d\n",params->inputheight);
	fprintf(g_fileproject, "OutputType\t\t=\t%d\n",params->outputtype);

	fprintf(g_fileproject, "VideoInfo\t\t=\t%d\n",params->videoinfoflag);
	fprintf(g_fileproject, "TimeStart\t\t=\t%d\n",params->timestart);
	fprintf(g_fileproject, "TimeEnd\t\t=\t%d\n",params->timeend);
	
	fprintf(g_fileproject, "KeyFrame\t\t=\t%d\n",params->keyframeflag);
	fprintf(g_fileproject, "KeyFrameTH\t\t=\t%d\n",params->keyframeth);

	fprintf(g_fileproject, "SectionFlag\t\t=\t%d\n",params->sectionflag);
	fprintf(g_fileproject, "SectionDuration\t\t=\t%d\n",params->sectionduration);
	fprintf(g_fileproject, "SectionNum\t\t=\t%d\n",params->sectionnum);
	fprintf(g_fileproject, "SectionCur\t\t=\t%d\n",params->sectioncur);
	
	fprintf(g_fileproject, "SegParaNoR\t\t=\t%d\n",params->segparanor);
	fprintf(g_fileproject, "SegParaWSD\t\t=\t%d\n",params->segparawsd);
	fprintf(g_fileproject, "SegParaNoI\t\t=\t%d\n",params->segparanoi);
	fprintf(g_fileproject, "SegParaAET\t\t=\t%d\n",params->segparaaet);
	fprintf(g_fileproject, "SegParaEEI\t\t=\t%d\n",params->segparaeei);
	fprintf(g_fileproject, "SegParaEES\t\t=\t%d\n",params->segparaees);
	fprintf(g_fileproject, "SegParaEET\t\t=\t%d\n",params->segparaeet);
	fprintf(g_fileproject, "SegParaEMM\t\t=\t%d\n",params->segparaemm);
	fprintf(g_fileproject, "SegParaEMR\t\t=\t%d\n",params->segparaemr);

	fprintf(g_fileproject, "InfParaIM\t\t=\t%d\n",params->infparaim);
	fprintf(g_fileproject, "InfParaIS\t\t=\t%d\n",params->infparais);
	 	
	fprintf(g_fileproject, "DepParaFILT\t\t=\t%d\n",params->depparafilt);
	fprintf(g_fileproject, "DepParaDFI\t\t=\t%d\n",params->depparadfi);
	fprintf(g_fileproject, "DepParaDFGSX\t\t=\t%d\n",params->depparadfgsx);
	fprintf(g_fileproject, "DepParaDFGSY\t\t=\t%d\n",params->depparadfgsy);
	fprintf(g_fileproject, "DepParaFEM\t\t=\t%d\n",params->depparafem);
	fprintf(g_fileproject, "DepParaEEM\t\t=\t%d\n",params->depparaeem);
	fprintf(g_fileproject, "DepParaEMR\t\t=\t%d\n",params->depparaemr);
	fprintf(g_fileproject, "DepParaARI\t\t=\t%d\n",params->depparaari);
	
	fprintf(g_fileproject, "RenParaOF\t\t=\t%d\n",params->renparaof);
	fprintf(g_fileproject, "RenParaRM\t\t=\t%d\n",params->renpararm);
	fprintf(g_fileproject, "RenParaRPP\t\t=\t%d\n",params->renpararpp);
	fprintf(g_fileproject, "RenParaALP\t\t=\t%d\n",params->renparaalp);
	fprintf(g_fileproject, "RenParaFP\t\t=\t%d\n",params->renparafp);
	fprintf(g_fileproject, "RenParaESM\t\t=\t%d\n",params->renparaesm);
	fprintf(g_fileproject, "RenParaESTH\t\t=\t%d\n",params->renparaesth);
	fprintf(g_fileproject, "RenParaFEM\t\t=\t%d\n",params->renparafem);
	fprintf(g_fileproject, "RenParaFER\t\t=\t%d\n",params->renparafer);
	fprintf(g_fileproject, "RenParaFERR\t\t=\t%d\n",params->renparaferr);
	fprintf(g_fileproject, "RenParaFEMM\t\t=\t%d\n",params->renparafemm);
	fprintf(g_fileproject, "RenParaFESP\t\t=\t%d\n",params->renparafesp);
	
	return ;
}

void NewProject(GtkWidget *widget,SCreateProject *data)
{	
	
	char cmd[256];
	char tmp[128];
	char str[32];
	
	gtk_widget_hide(GTK_WIDGET(g_wdgCreateNewProject));

	memset(tmp, 0, 128);
	memset(cmd, 0, 256);
	memset (&cfgparams, 0, sizeof (InputParameters));
	
	//Set default parameters.
	printf ("PROJECT  INFO --- Setting Default Parameters...\n");
	InitParams(ParaMap);
	
	sprintf_s(cfgparams.projectname, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entrypname)));

	printf ("PROJECT  INFO --- Creating project file %s.dpj.\n", cfgparams.projectname);

	sprintf_s(g_projectpath, "%s", cfgparams.projectname);
	if(_access(g_projectpath,0))
    {
        sprintf_s(cmd,"mkdir %s",g_projectpath);
        system(cmd);
		sprintf_s(cmd,"mkdir %s\\info",g_projectpath);
        system(cmd);
		sprintf_s(cmd,"mkdir %s\\depth",g_projectpath);
        system(cmd);
		sprintf_s(cmd,"mkdir %s\\render",g_projectpath);
        system(cmd);
		sprintf_s(cmd,"mkdir %s\\background",g_projectpath);
        system(cmd);
		sprintf_s(cmd,"mkdir %s\\bmp",g_projectpath);
        system(cmd);
        printf("PROJECT  INFO --- Create directory %s success！\n",g_projectpath);
    }
    else
    {
        printf("PROJECT ERROR --- The dirictory %s is exist\n",g_projectpath);
		messagebox(MSGBOX_PEXIST);
		return ;
    }

	sprintf_s(cmd,"./%s/%s.dpj",cfgparams.projectname,cfgparams.projectname);

	if((g_fileproject=fopen(cmd,"w"))==NULL)
	{
		printf("PROJECT ERROR --- creating project file failed!\n");
		return ;
	}
	else
	{
		sprintf_s(cfgparams.inputfile, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryinputfile)));
		
		sprintf_s(tmp, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryinwidth)));
		g_frameInWidth = cfgparams.inputwidth = str2num(tmp);

		sprintf_s(tmp, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryinheight)));
		g_frameInHeight = cfgparams.inputheight = str2num(tmp);

		sprintf_s(tmp, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryintimestart)));
		g_timestart = cfgparams.timestart = str2num(tmp);

		sprintf_s(tmp, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryintimeend)));
		g_timeend = cfgparams.timeend = str2num(tmp);

		sprintf_s(tmp, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryinsectionduration)));
		g_sectionduration = cfgparams.sectionduration = str2num(tmp);

		sprintf_s(tmp, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryinkeyframeth)));
		g_keyframeth = cfgparams.keyframeth = str2num(tmp);
		//保存关键帧信息
		sprintf_s(cfgparams.infofile, "./%s/%s", cfgparams.projectname,(char*)gtk_entry_get_text(GTK_ENTRY(data->entryinfofile)));

		sprintf_s(cfgparams.outputfile1, "./%s/%s", cfgparams.projectname,(char*)gtk_entry_get_text(GTK_ENTRY(data->entryoutputfile1)));
		sprintf_s(cfgparams.outputfile2, "./%s/%s", cfgparams.projectname,(char*)gtk_entry_get_text(GTK_ENTRY(data->entryoutputfile2)));
		//sprintf(cfgparams.h264encparam, "%s", (char*)gtk_entry_get_text(GTK_ENTRY(data->entryh264encparam)));
		
		cfgparams.inputtype = g_inputfileformat;
		cfgparams.outputtype = 0;

		cfgparams.videoinfoflag = 0;

		cfgparams.timestart = g_timestart;
		cfgparams.timeend = g_timeend;

		if(g_inputfileformat==INPIMG)
			g_keyframeflag = 0;
		cfgparams.keyframeflag = g_keyframeflag;
		cfgparams.keyframeth = g_keyframeth;

		cfgparams.sectionflag = g_sectionflag;
		cfgparams.sectionduration = g_sectionduration;
		cfgparams.sectionnum = 1;
		cfgparams.sectioncur = 1;
	
		cfgparams.segparanor = SEGPARAM_NUMREGIONDEF;
		cfgparams.segparawsd = SEGPARAM_WEIGHTSPADISDEF;
		cfgparams.segparanoi = SEGPARAM_ITERATEDEF;
		cfgparams.segparaaet = SEGPARAM_AUTOEDGETHDEF;
		cfgparams.segparaeei = SEGPARAM_EDGEENLARGEITERDEF;
		cfgparams.segparaees = SEGPARAM_EDGEENLARGESIZEDEF;
		cfgparams.segparaeet = SEGPARAM_EDGEENLARGETHDEF;
		cfgparams.segparaemm = SEGEDITMODULECIR;
		cfgparams.segparaemr = SEGEDITPARAM_RADDEF;

		cfgparams.infparaim = INFMODEPIX;
		cfgparams.infparais = INFPARAM_STRENGTHDEF;
	 	
		cfgparams.depparafilt= 0;;
		cfgparams.depparadfi = DEPFILTPARAM_DIDEF;
		cfgparams.depparadfgsx = DEPFILTPARAM_GSDEF;		
		cfgparams.depparadfgsy = DEPFILTPARAM_GSDEF;
		cfgparams.depparafem = DEPEDITMODEINCGRAD;
		cfgparams.depparaeem = DEPEDITMODULECIR;
		cfgparams.depparaemr = DEPEDITPARAM_RADDEF;
		cfgparams.depparaari = DEPAUTOPARAM_RIDEF;
		
		cfgparams.renparaof  = RENDEROUTLR;
		cfgparams.renpararm  = RENDERPUSH;
		cfgparams.renpararpp = RENDERPROCNONE;
		cfgparams.renparaalp = RENPARAM_ALPHADEF;
		cfgparams.renparafp  = RENPARAM_FPDEF;
		cfgparams.renparafem = RENEDITSELMODESQU;
		cfgparams.renparafer = RENEDITPARAM_SRADDEF;
		cfgparams.renparaferr= RENEDITPARAM_RRADDEF;
		cfgparams.renparafemm= RENEDITMATCHMOD9X9;
		cfgparams.renparafesp= RENEDITSELSPMENU;

		WriteProjectFile(&cfgparams);
		
		printf("PROJECT  INFO --- Creating project file successed!\n");

		fclose(g_fileproject);
	
		OpenInputFile(cfgparams.inputfile);

		//if(fopen_s(&g_projectSaveInfo, cfgparams.infofile,"wb")!=0)
		//{
			//printf("info file open failed!\n");
			//return ;
		//}
		//else
		//{
			//printf("info file open successed!\n");
		//}

		sprintf_s(str,"%d", SEGPARAM_AUTOEDGETHDEF);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditStrength), str);

		sprintf_s(str,"%d", RENPARAM_ALPHADEF);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditalpha), str);

		sprintf_s(str,"%d", RENPARAM_FPDEF);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditfocal), str);
		
		//fclose(g_projectSaveInfo);
	}	
	
	return ;
}

void CancelNewProject(GtkWidget *button,GtkWidget *window)
{
	gtk_widget_hide(g_wdgCreateNewProject);
}

void OpenProject(GtkWidget *widget,gpointer *data)
{
	char filename[128];
	InputParameters *p_Inp = &cfgparams;
	char *content = NULL;
	char str[32];

	if(g_isprojectopen==0)
		sprintf_s(filename,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgProjectOpenSelection)));
	else
		sprintf_s(filename,"%s/%s.dpj", cfgparams.projectname, cfgparams.projectname);
	if((g_fileproject=fopen(filename,"r"))==NULL)
    {
		printf("PROJECT ERROR --- Project open failed!\n");
		return ;
    }
    else
    {
		printf("PROJECT  INFO --- Project open successed!\n");
		if(g_isprojectopen==0)
			gtk_widget_hide(GTK_WIDGET(g_wdgProjectOpenSelection));
		memset (&cfgparams, 0, sizeof (InputParameters));
		//Set default parameters.
		printf ("PROJECT  INFO --- Setting Default Parameters...\n");
		InitParams(ParaMap);

		printf ("PROJECT  INFO --- Parsing project file %s\n", filename);
		content = GetProjectFileContent (filename);
		if (NULL==content)
		{
			printf("PROJECT ERROR --- Parsing project file %s Failed\n", filename);
			messagebox(MSGBOX_PPFFAIL);
			return ;
		}
		ParseContent (p_Inp, ParaMap, content, (int) strlen(content));
		//printf ("\n");
		free (content);

		//DisplayParams(ParaMap);
		/* open input file */
		g_inputfileformat = p_Inp->inputtype;
		g_frameInWidth = p_Inp->inputwidth;
		g_frameInHeight = p_Inp->inputheight;

		g_videoinfoflag = p_Inp->videoinfoflag;

		g_timestart = p_Inp->timestart;
		g_timeend = p_Inp->timeend;

		g_keyframeflag = p_Inp->keyframeflag;
		g_keyframeth = p_Inp->keyframeth;

		g_sectionflag = p_Inp->sectionflag;
		g_sectionduration = p_Inp->sectionduration;
		g_sectionnum = p_Inp->sectionnum;
		g_sectioncur = p_Inp->sectioncur;
		
		OpenInputFile(p_Inp->inputfile);

		/* open info file */
		if(g_keyframeflag==0)
		{
			sprintf_s(filename,"%s_%d", p_Inp->infofile, g_sectioncur);
			OpenFrameInfoFile(filename);
		}
		
		/* load parameters */
		LoadParameters(p_Inp);

		sprintf_s(str,"%d", g_segparamautoedgeth);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditStrength), str);

		sprintf_s(str,"%d", g_renderalpha);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditalpha), str);

		sprintf_s(str,"%d", g_renderforcalplane);
		gtk_entry_set_text(GTK_ENTRY(g_wdgEditfocal), str);
		
    }

	/*
	if(g_keyframeflag)
	{
		gdk_draw_gray_image (g_wdgKeyFrameDrawArea->window, g_wdgKeyFrameDrawArea->style->fg_gc[GTK_STATE_NORMAL],
          KFRMWIDTH, KFRMHEIGHT, KFRMWIDTH, KFRMHEIGHT,
          GDK_RGB_DITHER_MAX, g_depthdisplay, KFRMWIDTH);
	}
	*/
	
	return ;
}

void CancelProject(GtkWidget *widget,gpointer *data)
{
	gtk_widget_hide(GTK_WIDGET(g_wdgProjectOpenSelection));
}

void setinpformat0(GtkWidget *widget,gpointer *data)
{
	g_inputfileformat = INPYUV;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
}

void setinpformat1(GtkWidget *widget,gpointer *data)
{
	g_inputfileformat = INPIMG;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
}

void setsectionflag0(GtkWidget *widget,gpointer *data)
{
	g_sectionflag = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
}

void setsectionflag1(GtkWidget *widget,gpointer *data)
{
	g_sectionflag = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
}

void setkeyframeflag0(GtkWidget *widget,gpointer *data)
{
	g_keyframeflag = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
}

void setkeyframeflag1(GtkWidget *widget,gpointer *data)
{
	g_keyframeflag = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
}


void setinpformat2(GtkWidget *widget,gpointer *data)
{
	g_inputfileformat = INPVID;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
}



void selectinfilename(GtkWidget *widget,gpointer *data)
{
	sprintf_s(cfgparams.inputfile,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgInputfileOpenSelection)));
	gtk_entry_set_text(GTK_ENTRY(data), cfgparams.inputfile);
	gtk_widget_hide(GTK_WIDGET(g_wdgInputfileOpenSelection));
}

void cancelinfilename(GtkWidget *widget,gpointer *data)
{
	gtk_widget_hide(GTK_WIDGET(g_wdgInputfileOpenSelection));
}

void selectinputfilename(GtkWidget *widget,gpointer *data)
{
	g_wdgInputfileOpenSelection = gtk_file_selection_new(FILE_SELECT);/*创建文件选择构件*/
	gtk_window_set_position(GTK_WINDOW(g_wdgInputfileOpenSelection),GTK_WIN_POS_NONE);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(g_wdgInputfileOpenSelection),"*.");
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgInputfileOpenSelection))->ok_button),"clicked",GTK_SIGNAL_FUNC(selectinfilename),data);
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgInputfileOpenSelection))->cancel_button),"clicked",GTK_SIGNAL_FUNC(cancelinfilename),NULL);
	gtk_widget_show(g_wdgInputfileOpenSelection);
}

void selectinfofilename(GtkWidget *widget,gpointer *data)
{
	sprintf_s(cfgparams.infofile,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgInfofileOpenSelection)));
	gtk_entry_set_text(GTK_ENTRY(data), cfgparams.infofile);
	gtk_widget_hide(GTK_WIDGET(g_wdgInfofileOpenSelection));
}

void cancelinfofilename(GtkWidget *widget,gpointer *data)
{
	gtk_widget_hide(GTK_WIDGET(g_wdgInfofileOpenSelection));
}

void selectinformationfilename(GtkWidget *widget,gpointer *data)
{
	g_wdgInfofileOpenSelection = gtk_file_selection_new(FILE_SELECT);/*创建文件选择构件*/
	gtk_window_set_position(GTK_WINDOW(g_wdgInfofileOpenSelection),GTK_WIN_POS_NONE);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(g_wdgInfofileOpenSelection),"*.");
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgInfofileOpenSelection))->ok_button),"clicked",GTK_SIGNAL_FUNC(selectinfofilename),data);
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgInfofileOpenSelection))->cancel_button),"clicked",GTK_SIGNAL_FUNC(cancelinfofilename),NULL);
	gtk_widget_show(g_wdgInfofileOpenSelection);
}

void selectoutfilename(GtkWidget *widget,gpointer *data)
{
	sprintf_s(cfgparams.outputfile1,"%s",gtk_file_selection_get_filename(GTK_FILE_SELECTION(g_wdgOutputfileOpenSelection)));
	gtk_entry_set_text(GTK_ENTRY(data), cfgparams.outputfile1);
	gtk_widget_hide(GTK_WIDGET(g_wdgOutputfileOpenSelection));
}

void canceloutfilename(GtkWidget *widget,gpointer *data)
{
	gtk_widget_hide(GTK_WIDGET(g_wdgOutputfileOpenSelection));
}

void selectoutputfilename(GtkWidget *widget,gpointer *data)
{
	g_wdgOutputfileOpenSelection = gtk_file_selection_new(FILE_SELECT);/*创建文件选择构件*/
	gtk_window_set_position(GTK_WINDOW(g_wdgOutputfileOpenSelection),GTK_WIN_POS_NONE);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(g_wdgOutputfileOpenSelection),"*.");
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgOutputfileOpenSelection))->ok_button),"clicked",GTK_SIGNAL_FUNC(selectoutfilename),data);
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgOutputfileOpenSelection))->cancel_button),"clicked",GTK_SIGNAL_FUNC(canceloutfilename),NULL);
	gtk_widget_show(g_wdgOutputfileOpenSelection);
}

void newprojectfile(GtkWidget * button, GtkWidget * data)
{
	char buffer[128];

	GtkWidget *entryprojname, *entryinputfile, *entryinfofile, *entryoutputfile1, *entryoutputfile2;// *entryh264encparam; 
	GtkWidget *entryinwidth, *entryinheight, *inputformat0, *inputformat1, *inputformat2;
	GtkWidget *entryintimestart, *entryintimeend, *entryinsectionduration, *sectionflag0, *sectionflag1, *keyframeflag0, *keyframeflag1, *entryinkeyframeth;
	GtkWidget *selectinfile;//, *selectinfofile, *selectoutfile;
    GtkWidget *label,*vbox,*hbox,*b_ok,*b_cancel, *separator;
	SCreateProject *wdv;
	
	g_wdgCreateNewProject=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(g_wdgCreateNewProject,720,480);
    gtk_window_set_title(GTK_WINDOW(g_wdgCreateNewProject),PROJ_NEW);
    g_signal_connect(G_OBJECT(g_wdgCreateNewProject),"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
    gtk_window_set_position(GTK_WINDOW(g_wdgCreateNewProject),GTK_WIN_POS_NONE);

    vbox=gtk_vbox_new(FALSE,15);
    gtk_container_add(GTK_CONTAINER(g_wdgCreateNewProject),vbox);

	/* project name */
	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_NAME);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryprojname = gtk_entry_new();
	gtk_widget_set_size_request( entryprojname,70, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryprojname,FALSE,FALSE,0);
	sprintf_s(buffer, "test");
	gtk_entry_set_text(GTK_ENTRY(entryprojname), buffer);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	/* input */
	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_INPFN);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryinputfile = gtk_entry_new();
	gtk_widget_set_size_request( entryinputfile,480, 25);
    gtk_box_pack_start(GTK_BOX(hbox),entryinputfile,FALSE,FALSE,0);
	sprintf_s(buffer, "test.yuv");
	gtk_entry_set_text(GTK_ENTRY(entryinputfile), buffer);

	selectinfile = gtk_button_new_with_label(" ... ");
	gtk_box_pack_start(GTK_BOX(hbox),selectinfile, FALSE,FALSE,0);
	g_signal_connect( G_OBJECT(selectinfile), "clicked", G_CALLBACK(selectinputfilename), entryinputfile );

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_INPF);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	inputformat2 = gtk_radio_button_new_with_label(NULL, PROJ_NEW_INPFV);
	gtk_box_pack_start(GTK_BOX(hbox),inputformat2,FALSE,FALSE,0);
	inputformat0 = gtk_radio_button_new_with_label(NULL, PROJ_NEW_INPFY);
	gtk_box_pack_start(GTK_BOX(hbox),inputformat0,FALSE,FALSE,0);
	inputformat1 = gtk_radio_button_new_with_label(NULL, PROJ_NEW_INPFI);
	gtk_box_pack_start(GTK_BOX(hbox),inputformat1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(inputformat1),gtk_radio_button_group(GTK_RADIO_BUTTON(inputformat2)));
	gtk_radio_button_set_group(GTK_RADIO_BUTTON(inputformat0),gtk_radio_button_group(GTK_RADIO_BUTTON(inputformat2)));

	g_signal_connect( G_OBJECT(inputformat0), "released", G_CALLBACK(setinpformat0), inputformat0 );
	g_signal_connect( G_OBJECT(inputformat1), "released", G_CALLBACK(setinpformat1), inputformat1 );
	g_signal_connect( G_OBJECT(inputformat2), "released", G_CALLBACK(setinpformat2), inputformat2 );

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_INPR);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	
    entryinwidth = gtk_entry_new_with_max_length(8);
	sprintf_s(buffer,"%d", g_frameInWidth);
    gtk_entry_set_text(GTK_ENTRY(entryinwidth), buffer);
	gtk_entry_set_alignment(GTK_ENTRY(entryinwidth), 1);
	gtk_widget_set_size_request( entryinwidth,70, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryinwidth,FALSE,FALSE,0);

	label=gtk_label_new(" x ");
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	entryinheight = gtk_entry_new_with_max_length(8);
	sprintf_s(buffer,"%d", g_frameInHeight);
    gtk_entry_set_text(GTK_ENTRY(entryinheight), buffer);
	gtk_entry_set_alignment(GTK_ENTRY(entryinheight), 1);
	gtk_widget_set_size_request( entryinheight,70, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryinheight,FALSE,FALSE,0);
	
	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);
	
	/* info file */
	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_IFN);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryinfofile = gtk_entry_new();
	gtk_widget_set_size_request( entryinfofile,480, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryinfofile,FALSE,FALSE,0);
	sprintf_s(buffer, "test.dat");
	gtk_entry_set_text(GTK_ENTRY(entryinfofile), buffer);

	//selectinfofile = gtk_button_new_with_label(" ... ");
	//gtk_box_pack_start(GTK_BOX(hbox),selectinfofile, FALSE,FALSE,0);
	//g_signal_connect( G_OBJECT(selectinfofile), "clicked", G_CALLBACK(selectinformationfilename), entryinfofile );

	/* output file */
	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_OFN);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryoutputfile1 = gtk_entry_new();
	gtk_widget_set_size_request( entryoutputfile1,230, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryoutputfile1,FALSE,FALSE,0);
	sprintf_s(buffer, "test_3d.yuv");
	gtk_entry_set_text(GTK_ENTRY(entryoutputfile1), buffer);

	entryoutputfile2 = gtk_entry_new();
	gtk_widget_set_size_request( entryoutputfile2,230, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryoutputfile2,FALSE,FALSE,0);
	sprintf_s(buffer, "test_3d.mkv");
	gtk_entry_set_text(GTK_ENTRY(entryoutputfile2), buffer);

	/* video section */
	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),separator,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_VD);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	label=gtk_label_new(PROJ_NEW_FROM);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	
    entryintimestart = gtk_entry_new_with_max_length(8);
	sprintf_s(buffer,"%d", g_timestart);
    gtk_entry_set_text(GTK_ENTRY(entryintimestart), buffer);
	gtk_entry_set_alignment(GTK_ENTRY(entryintimestart), 1);
	gtk_widget_set_size_request( entryintimestart,70, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryintimestart,FALSE,FALSE,0);

	label=gtk_label_new(PROJ_NEW_TO);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);

	entryintimeend = gtk_entry_new_with_max_length(8);
	sprintf_s(buffer,"%d", g_timeend);
    gtk_entry_set_text(GTK_ENTRY(entryintimeend), buffer);
	gtk_entry_set_alignment(GTK_ENTRY(entryintimeend), 1);
	gtk_widget_set_size_request( entryintimeend,70, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryintimeend,FALSE,FALSE,0);

	
	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	sectionflag0 = gtk_radio_button_new_with_label(NULL, PROJ_NEW_NS);
	gtk_box_pack_start(GTK_BOX(hbox),sectionflag0,FALSE,FALSE,0);
	sectionflag1 = gtk_radio_button_new_with_label(NULL, PROJ_NEW_HS);
	gtk_box_pack_start(GTK_BOX(hbox),sectionflag1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(sectionflag0),gtk_radio_button_group(GTK_RADIO_BUTTON(sectionflag1)));

	g_signal_connect( G_OBJECT(sectionflag0), "released", G_CALLBACK(setsectionflag0), sectionflag0 );
	g_signal_connect( G_OBJECT(sectionflag1), "released", G_CALLBACK(setsectionflag1), sectionflag1 );
	
	//hbox=gtk_hbox_new(FALSE,50);
	//gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new(PROJ_NEW_SD);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	
    entryinsectionduration = gtk_entry_new_with_max_length(8);
	sprintf_s(buffer,"%d", g_sectionduration);
    gtk_entry_set_text(GTK_ENTRY(entryinsectionduration), buffer);
	gtk_entry_set_alignment(GTK_ENTRY(entryinsectionduration), 1);
	gtk_widget_set_size_request( entryinsectionduration,70, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryinsectionduration,FALSE,FALSE,0);

	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	keyframeflag0 = gtk_radio_button_new_with_label(NULL, PROJ_NEW_NKM);
	gtk_box_pack_start(GTK_BOX(hbox),keyframeflag0,FALSE,FALSE,0);
	keyframeflag1 = gtk_radio_button_new_with_label(NULL, PROJ_NEW_KM);
	gtk_box_pack_start(GTK_BOX(hbox),keyframeflag1,FALSE,FALSE,0);

	gtk_radio_button_set_group(GTK_RADIO_BUTTON(keyframeflag0),gtk_radio_button_group(GTK_RADIO_BUTTON(keyframeflag1)));

	g_signal_connect( G_OBJECT(keyframeflag0), "released", G_CALLBACK(setkeyframeflag0), keyframeflag0 );
	g_signal_connect( G_OBJECT(keyframeflag1), "released", G_CALLBACK(setkeyframeflag1), keyframeflag1 );

	label=gtk_label_new(PROJ_NEW_KDT);
    gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	
    entryinkeyframeth = gtk_entry_new_with_max_length(8);
	sprintf_s(buffer,"%d", g_keyframeth);
    gtk_entry_set_text(GTK_ENTRY(entryinkeyframeth), buffer);
	gtk_entry_set_alignment(GTK_ENTRY(entryinkeyframeth), 1);
	gtk_widget_set_size_request( entryinkeyframeth,70, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryinkeyframeth,FALSE,FALSE,0);

	/*
	hbox=gtk_hbox_new(FALSE,50);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	label=gtk_label_new("H264 Encode Parameters: ");
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	entryh264encparam = gtk_entry_new();
	gtk_widget_set_size_request( entryh264encparam,400, 25);
	gtk_box_pack_start(GTK_BOX(hbox),entryh264encparam,FALSE,FALSE,0);
	sprintf(buffer, "-q 18");
	gtk_entry_set_text(GTK_ENTRY(entryh264encparam), buffer);
	*/

	//selectoutfile = gtk_button_new_with_label(" ... ");
	//gtk_box_pack_start(GTK_BOX(hbox),selectoutfile, FALSE,FALSE,0);
	//g_signal_connect( G_OBJECT(selectoutfile), "clicked", G_CALLBACK(selectoutputfilename), entryoutputfile );

	wdv =(SCreateProject *)malloc(sizeof(SCreateProject));
	wdv->entrypname = entryprojname;
	wdv->entryinputfile = entryinputfile;
	wdv->entryinwidth = entryinwidth;
	wdv->entryinheight = entryinheight;
	wdv->entryintimestart = entryintimestart;
	wdv->entryintimeend = entryintimeend;
	wdv->entryinsectionduration = entryinsectionduration;
	wdv->entryinkeyframeth = entryinkeyframeth;
	wdv->entryinfofile = entryinfofile;
	wdv->entryoutputfile1 = entryoutputfile1;
	wdv->entryoutputfile2 = entryoutputfile2;
	//wdv->entryh264encparam = entryh264encparam;
	
	/* ok and cancel */
	hbox=gtk_hbox_new(FALSE,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
    b_ok=gtk_button_new_with_label(CONF_OK);
    g_signal_connect(G_OBJECT(b_ok),"clicked",G_CALLBACK(NewProject),wdv);
    gtk_box_pack_start(GTK_BOX(hbox),b_ok,FALSE,FALSE,50);
    b_cancel=gtk_button_new_with_label(CONF_CANCEL);
    g_signal_connect(G_OBJECT(b_cancel),"clicked",G_CALLBACK(CancelNewProject),g_wdgCreateNewProject);
    gtk_box_pack_start(GTK_BOX(hbox),b_cancel,FALSE,FALSE,0);

    gtk_widget_show_all(g_wdgCreateNewProject);
	return ;
}

void openprojectfile(GtkWidget * button, GtkWidget * data)
{
	g_wdgProjectOpenSelection=gtk_file_selection_new(PROJ_OPEN_SEL);/*创建文件选择构件*/
	gtk_window_set_position(GTK_WINDOW(g_wdgProjectOpenSelection),GTK_WIN_POS_NONE);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(g_wdgProjectOpenSelection),"*.dpj");
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgProjectOpenSelection))->ok_button),"clicked",GTK_SIGNAL_FUNC(OpenProject),NULL);
	gtk_signal_connect(GTK_OBJECT((GTK_FILE_SELECTION(g_wdgProjectOpenSelection))->cancel_button),"clicked",GTK_SIGNAL_FUNC(CancelProject),NULL);
	gtk_widget_show(g_wdgProjectOpenSelection);

	return ;
}

void saveprojectfile(GtkWidget * button, GtkWidget * data)
{
	int i=0;
	char filename[256];
	if(g_fileproject==NULL)
	{
		// donothing
	}
	else
	{
		// save project file
		sprintf_s(filename,"./%s/%s.dpj",cfgparams.projectname,cfgparams.projectname);
		if((g_fileproject = fopen(filename,"w"))==NULL)
		{
			printf("PROJECT ERROR --- Saving project file failed!\n");
			return ;
		}
		else
		{
			cfgparams.inputtype = g_inputfileformat;
			cfgparams.inputwidth = g_frameInWidth;
			cfgparams.inputheight = g_frameInHeight;
			cfgparams.outputtype = 0;

			cfgparams.videoinfoflag = g_videoinfoflag;

			cfgparams.timestart = g_timestart;
			cfgparams.timeend = g_timeend;
			
			cfgparams.keyframeflag = g_keyframeflag;
			cfgparams.keyframeth = g_keyframeth;

			cfgparams.sectionflag = g_sectionflag;
			cfgparams.sectionduration = g_sectionduration;
			cfgparams.sectionnum = g_sectionnum;
			cfgparams.sectioncur = g_sectioncur;

			cfgparams.segparanor = g_segparamnumregion;
			cfgparams.segparawsd = g_segparamweightspadis;
			cfgparams.segparanoi = g_segparamiterate;
			cfgparams.segparaaet = g_segparamautoedgeth;
			cfgparams.segparaeei = g_segparamedgeenlargeiter;
			cfgparams.segparaees = g_segparamedgeenlargesize;
			cfgparams.segparaeet = g_segparamedgeenlargeth;
			cfgparams.segparaemm = g_segeditmodule;
			cfgparams.segparaemr = g_segeditmodulerad;

			cfgparams.infparaim = g_inferencemode;
			cfgparams.infparais = g_inferencestrength;
		 	
			cfgparams.depparafilt= g_depthprocessparam;
			cfgparams.depparadfi = g_depfiltdilateiter;
			cfgparams.depparadfgsx = g_depfiltgausesigmax;
			cfgparams.depparadfgsy = g_depfiltgausesigmay;
			cfgparams.depparafem = g_depeditmode;
			cfgparams.depparaeem = g_depeditmodule;
			cfgparams.depparaemr = g_depeditmodulerad;
			cfgparams.depparaari = g_depautorefineiter;
		
			cfgparams.renparaof  = g_renderoutputformat;
			cfgparams.renpararm  = g_rendermethod;
			cfgparams.renpararpp = g_renderprocmethod;
			cfgparams.renparaalp = g_renderalpha;
			cfgparams.renparaesm = g_renderedgesoftenmethod;
			cfgparams.renparaesth= g_renderedgesoftenth;
			cfgparams.renparafp  = g_renderforcalplane;
			cfgparams.renparafem = g_rendereditselmodule;
			cfgparams.renparafer = g_rendereditselmodulerad;
			cfgparams.renparaferr= g_rendereditrefregionrad;
			cfgparams.renparafemm= g_rendereditmatchmodule;
			cfgparams.renparafesp= g_rendereditstartpoint;

			WriteProjectFile(&cfgparams);
			
			printf("PROJECT  INFO --- Saving project file successed!\n");

			fclose(g_fileproject);
		}
		
		// save info file
		if(g_frames[g_frameIndex].SegFlag)// &&g_frames[g_frameIndex].ChangeFlag==1) // TODO 可优化，判断是否修改
		{
			if(g_keyframeflag)
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_keyframenumarr[g_frameIndex]+1);
			else
				sprintf(filename,"./%s/info/%d_infosnap_%d.dat",cfgparams.projectname,g_sectioncur,g_frameIndex+1);
			if((g_frames[g_frameIndex].finfo=fopen(filename,"wb"))!=NULL)
			{
				// save info file
				saveinfofile(g_frames[g_frameIndex].finfo, g_frameIndex);
				fclose(g_frames[g_frameIndex].finfo);
			}
		}

		// save frame info file
		if(g_keyframeflag==0)
		{
			sprintf_s(filename,"%s_%d",cfgparams.infofile, g_sectioncur);
			if((g_projectSaveInfo=fopen(filename,"wb"))==NULL)
			{
				printf("PROJECT ERROR --- Saving frame info file failed!\n");
				return ;
			}
			else
			{
				for(i=0;i<g_frameMaxNum;i++)
				{
					if(g_frames[i].KeyFlag==1)
					{
						fwrite(&i, 4, 1, g_projectSaveInfo); // frame index
					}
				}

				printf("PROJECT  INFO --- Saving frame info file successed!\n");
				
				fclose(g_projectSaveInfo);
			}
		}
	}

	return ;
}

void closeprojectfile(GtkWidget * button, GtkWidget * data)
{
	if(g_frameMaxNum>0)
	{
		// 显示区
		uint8_t * tmp = new uint8_t [IMGWIDTHMAX*IMGHEIGHTMAX*4];
		// 背景颜色为黑色
		for(int i=0;i<IMGWIDTHMAX*IMGHEIGHTMAX*4;i++)
			tmp[i] = 238;
		gdk_draw_gray_image (g_wdgImgProcArea->window, g_wdgImgProcArea->style->fg_gc[GTK_STATE_NORMAL],
	          0, 0, IMGWIDTHMAX*2, IMGHEIGHTMAX*2,
	          GDK_RGB_DITHER_MAX, tmp, IMGWIDTHMAX*2);
		if(tmp) delete [] tmp;

		// layer 区
		

		// 模式区
		g_renderflag = 0;
		g_renderdisplayflag = 0;
		g_enlargeflag = 0;
		g_segeditflag = 0;
		g_mateditflag = 0;
		g_matselflag = 0;
		g_depeditflag = 0;
		g_rendereditflag = 0;
		
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgSegmentMode), TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_wdgNonEdit), TRUE);

		// 信息区
		
		//g_wdgFrameNum = gtk_label_new("   0 /	0");
		//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgFrameNum, FRMORGX+FRMOFFSET+3*FRMOFFSETX, FRMORGY+FRMOFFSETY+5);
		
		//gtk_entry_set_text(GTK_ENTRY(g_wdgInpFrameNum), "0");
		
		//g_wdgKeyFrameNum = gtk_label_new("	 0	-  0 -	0  ");
		//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgKeyFrameNum, KFRMORGX+KFRMOFFSET+2*KFRMOFFSETX-10, KFRMORGY+KFRMOFFSETY+5);

		//g_wdgInfo = gtk_label_new("   Coordinate Position X: 0 Y: 0   /   Color R: 0 G: 0 B: 0   /   Layer: 0   /   Depth: 0  /  Render Ref: 0  ");
    	//gtk_fixed_put(GTK_FIXED(g_wdgMainInterface),g_wdgInfo, INFOORGX, INFOORGY);
	
		// 保存文件
		saveprojectfile(button, data);

		// 释放内存
		closeyuvfile();
	}

	return ;
}



