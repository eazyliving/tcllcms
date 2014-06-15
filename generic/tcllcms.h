/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* includes,includes,includes
*
************************************************************************/

#ifdef _WINDOWS
#include <windows.h>
#endif
#define PACKAGE "tcllcms"
#define VERSION "0.27"

#define DEFINE_CMD(func) \
static int func(clientData, interp, objc, objv) \
    ClientData clientData; \
    Tcl_Interp *interp;	\
    int objc; \
    Tcl_Obj *CONST objv[];

#define DEFINE_CMD_E(func) \
extern int func(clientData, interp, objc, objv) \
    ClientData clientData; \
    Tcl_Interp *interp;	\
    int objc; \
    Tcl_Obj *CONST objv[];


#define ADD_CMD(cmdName, cmdProc) \
Tcl_CreateObjCommand(interp, cmdName, (Tcl_ObjCmdProc *) cmdProc, NULL, tcllcms_delete)

#define DECLARE_CMD(func) \
static int func _ANSI_ARGS_((ClientData clientData, \
		   Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]))

#define DECLARE_CMD_E(func) \
extern int func _ANSI_ARGS_((ClientData clientData, \
		   Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]))


#include <tcl.h>
#include <string.h>
#include <stdlib.h>
#include <lcms.h>

typedef struct {
		Tcl_Interp *interp;
		char *script;
} tcllcms_ErrorHandler; 

typedef struct {
        cmsHPROFILE input;
        cmsHPROFILE output;
	    cmsHPROFILE proof;
		char *input_handle;
		char *output_handle;
		char *proofing_handle;
		cmsHTRANSFORM transform;
		int intent;
		int proofingintent;
		BOOL proofingtrans;
} transform_data;

DECLARE_CMD_E(tcl_cmsDeltaE);
DECLARE_CMD_E(tcl_cmsOpenProfile);
DECLARE_CMD_E(tcl_cmsSaveProfile);
DECLARE_CMD_E(tcl_cmsCreateTransform);
DECLARE_CMD_E(tcl_cmsDoTransform);
DECLARE_CMD_E(tcl_cmsConversion);
DECLARE_CMD_E(tcl_cmsErrorAction);
DECLARE_CMD_E(tcl_cmsSetErrorHandler);
DECLARE_CMD_E(tcl_cmsGetErrorHandler);
DECLARE_CMD_E(tcl_cmsWhitePointFromTemp);
DECLARE_CMD_E(tcl_cmsSetCMYKNormalized);
DECLARE_CMD_E(tcl_cmsBuildGamma);
DECLARE_CMD_E(tcl_cmsBuildParametricGamma);
DECLARE_CMD_E(tcl_cmsAllocGamma);
DECLARE_CMD_E(tcl_cmsReverseGamma);
DECLARE_CMD_E(tcl_cmsJoinGamma);
DECLARE_CMD_E(tcl_cmsSmoothGamma);
DECLARE_CMD_E(tcl_cmsCreateGrayProfile);
DECLARE_CMD_E(tcl_cmsClampLab);
DECLARE_CMD_E(tcl_cmsTransform2DeviceLink);
DECLARE_CMD_E(tcl_cmsTransformImage);




static int tcl_ErrorHandler (int ErrorCode, const char *ErrorText);

DECLARE_CMD_E(ObjProfile);
DECLARE_CMD_E(ObjTransform);
DECLARE_CMD_E(ObjGamma);

extern void ObjTransformDelete (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);
extern void ObjProfileDelete (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);
extern void ObjGammaDelete (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);
extern cmsHTRANSFORM getTransFromObj (Tcl_Interp *interp, char *cmdname); 
extern ClientData getDataFromObj (Tcl_Interp *interp, char *cmdname);

extern cmsHPROFILE getProfileFromObj (Tcl_Interp *interp, char *cmdname);


static int lastprofilenum=0;
static int lasttransnum=0;
static int lastgammanum=0;
static int cmyk_normalized=0;
static tcllcms_ErrorHandler tcl_EHandler={ (Tcl_Interp *) NULL ,(char * ) NULL};

extern void getLabFromList (LPcmsCIELab Lab,Tcl_Obj **list);
extern void getXYZFromList (LPcmsCIEXYZ XYZ,Tcl_Obj **list);
extern void getLChFromList (LPcmsCIELCh LCh,Tcl_Obj **list);
extern void getxyYFromList (LPcmsCIExyY xyY,Tcl_Obj **list);
extern void getValFromList (WORD input[],Tcl_Obj **list,int count,double norm);
extern void getDoubleFromList (double input[],Tcl_Obj **list,int count);
