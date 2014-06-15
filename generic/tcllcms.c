/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* This is the main stuff...
* 
************************************************************************/


#include "tcllcms.h"

static void tcllcms_delete(ClientData clientData) {
	
}

#ifdef _WINDOWS
__declspec( dllexport )
#endif
int Tcllcms_Init (Tcl_Interp * interp)
    
{
 
 if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
    return TCL_ERROR;
  if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL)
    return TCL_ERROR;
  if (Tcl_PkgProvide(interp, "tcllcms" , VERSION) != TCL_OK)
    return TCL_ERROR;

  
  ADD_CMD( "cmsDeltaE", tcl_cmsDeltaE);
  ADD_CMD( "cmsOpenProfile",tcl_cmsOpenProfile);
  ADD_CMD( "cmsSaveProfile",tcl_cmsSaveProfile);
  ADD_CMD( "cmsCreateTransform",tcl_cmsCreateTransform);
  ADD_CMD( "cmsCreateProofingTransform",tcl_cmsCreateTransform);
  ADD_CMD( "cmsCreateMultiprofileTransform",tcl_cmsCreateTransform);
  ADD_CMD( "cmsDoTransform",tcl_cmsDoTransform);
  ADD_CMD( "cmsTransform2DeviceLink",tcl_cmsTransform2DeviceLink);
  ADD_CMD( "cmsConversion",tcl_cmsConversion);
  ADD_CMD( "cmsErrorAction",tcl_cmsErrorAction);
  ADD_CMD( "cmsSetErrorHandler",tcl_cmsSetErrorHandler);
  ADD_CMD( "cmsGetErrorHandler",tcl_cmsGetErrorHandler);
  ADD_CMD( "cmsWhitePointFromTemp",tcl_cmsWhitePointFromTemp); 	
  ADD_CMD( "cmsSetCMYKNormalized",tcl_cmsSetCMYKNormalized);
  ADD_CMD( "cmsBuildGamma", tcl_cmsBuildGamma);
  ADD_CMD( "cmsBuildParametricGamma",tcl_cmsBuildParametricGamma);
  ADD_CMD( "cmsAllocGamma",tcl_cmsAllocGamma);
  ADD_CMD( "cmsReverseGamma", tcl_cmsReverseGamma);
  ADD_CMD( "cmsJoinGamma",tcl_cmsJoinGamma);
  ADD_CMD( "cmsSmoothGamma",tcl_cmsSmoothGamma);
  ADD_CMD( "cmsCreateGrayProfile",tcl_cmsCreateGrayProfile);
  ADD_CMD( "cmsClampLab",tcl_cmsClampLab);
  ADD_CMD( "cmsTransformImage",tcl_cmsTransformImage);



	
 return TCL_OK;

}
#ifdef _WINDOWS
__declspec( dllexport )
#endif
int Tcllcms_SafeInit (interp)
    Tcl_Interp *interp;
{
  return Tcllcms_Init(interp);
}
