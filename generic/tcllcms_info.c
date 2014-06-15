/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* profile information functions
* 
************************************************************************/

#include "tcllcms.h"

void css2result(Tcl_Interp *interp,icColorSpaceSignature css);
void deviceclass2result(Tcl_Interp *interp,icProfileClassSignature css);

DEFINE_CMD_E(ObjProfile) {

    cmsHPROFILE profile;
	
	char * deviceclass_types[]={"scanner","monitor","printer","link","abstract","colorspace",
						"namedcolorspace","scnr","mntr","prtr","abst","spac","nmcl",(char *)NULL};
	
	icProfileClassSignature deviceclass_typevals[]={icSigInputClass,icSigDisplayClass,icSigOutputClass,icSigLinkClass,
						icSigAbstractClass,icSigColorSpaceClass,icSigNamedColorClass,
						icSigInputClass,icSigDisplayClass,icSigOutputClass,
						icSigAbstractClass,icSigColorSpaceClass,icSigNamedColorClass};
		
	char * colorspace_types[]={"XYZ","Lab","Luv","YCbr","Yxy","RGB","GRAY","HSV","HLS","CMYK","CMY","2CLR","3CLR","4CLR","5CLR","6CLR","7CLR","8CLR","ACLR","BCLR","DCLR","ECLR","FCLR",(char *)NULL};
	icColorSpaceSignature colorspace_typevals[]={
			icSigXYZData,icSigLabData,icSigLuvData,icSigYCbCrData,icSigYxyData,icSigRgbData,icSigGrayData,icSigHsvData,
			icSigHlsData,icSigCmykData,icSigCmyData,icSig2colorData,icSig3colorData,icSig4colorData,icSig5colorData,icSig6colorData,
			icSig7colorData,icSig8colorData,icSig9colorData,icSig10colorData,icSig11colorData,icSig12colorData,icSig13colorData,
			icSig14colorData,icSig15colorData};


	char *commands[]={"delete","get","set"};
	
	char *get_switches[]={
			"productname",
			"productinfo",
			"manufacturer",
			"model",
			"pcs",
			"colorspace",
			"deviceclass",
			"renderingintent",
			"isintentsupported",
			"mediawhitepoint",
			"mediablackpoint",
			"iluminant",
			"colorants",
			(char *)NULL
	};

	char *set_switches[]={
			"pcs",
			"colorspace",
			"deviceclass",
			"renderingintent",
			"tag",
			(char *)NULL
	};

    

	int pcommand_idx,command_idx,intent,direction,idx,intval;
	char buffer[255];

	cmsCIEXYZ XYZ;
	cmsCIEXYZTRIPLE XYZtriple;
	Tcl_Obj *list,*colorants[3];

	if (objc==1) {
		Tcl_WrongNumArgs(interp,1,objv,"command ?option?");
		return TCL_ERROR;
	}
	
	if (Tcl_GetIndexFromObj(interp,objv[1],commands,"command",(int)NULL,&pcommand_idx)==TCL_ERROR) return TCL_ERROR;	
    
	profile=(cmsHPROFILE) clientData;
			
	switch (pcommand_idx) {
		case 0:
			Tcl_DeleteCommand(interp,Tcl_GetString(objv[0]));
			return TCL_OK;
			break;

		case 1:
			if (Tcl_GetIndexFromObj(interp,objv[2],get_switches,"option",(int)NULL,&command_idx)==TCL_ERROR) return TCL_ERROR;
				
			// cycle get-commands...
			
		
			switch (command_idx) {
				
				case 0:
					Tcl_SetResult(interp,(char *)cmsTakeProductName(profile),TCL_VOLATILE);
					break;
				case 1:
					Tcl_SetResult(interp,(char *)cmsTakeProductInfo(profile),TCL_VOLATILE);
					break;
				case 2:
					Tcl_SetResult(interp,(char *)cmsTakeManufacturer(profile),TCL_VOLATILE);
					break;
				case 3:
					Tcl_SetResult(interp,(char *)cmsTakeModel(profile),TCL_VOLATILE);
					break;
				case 4:
					css2result(interp,cmsGetPCS(profile));
					break;
				case 5:
					css2result(interp,cmsGetColorSpace(profile));
					break;
				case 6:
					deviceclass2result(interp,cmsGetDeviceClass(profile));
					break;
				case 7:
					sprintf(buffer,"%d",cmsTakeRenderingIntent(profile));
					Tcl_SetResult(interp,buffer,TCL_VOLATILE);
					break;
				case 8:
					// cmsIsIntentSupported
					if (objc!=5) {
						Tcl_WrongNumArgs(interp,3,objv,"intent direction");
						return TCL_ERROR;
						}
					intent=atoi(Tcl_GetString(objv[3]));
					direction=atoi(Tcl_GetString(objv[4]));
					if (cmsIsIntentSupported(profile,intent,direction)==FALSE) 
						Tcl_SetResult(interp,"0",TCL_STATIC);
					else Tcl_SetResult(interp,"1",TCL_STATIC);
					break;
				case 9:
					cmsTakeMediaWhitePoint(&XYZ,profile);
					sprintf(buffer,"%.6f %.6f %.6f",XYZ.X,XYZ.Y,XYZ.Z);
					Tcl_SetResult(interp,buffer,TCL_VOLATILE);
					break;
				case 10:
					cmsTakeMediaBlackPoint(&XYZ,profile);
					sprintf(buffer,"%.6f %.6f %.6f",XYZ.X,XYZ.Y,XYZ.Z);
					Tcl_SetResult(interp,buffer,TCL_VOLATILE);
					break;
				case 11:
					cmsTakeIluminant(&XYZ,profile);
					sprintf(buffer,"%.6f %.6f %.6f",XYZ.X,XYZ.Y,XYZ.Z);
					Tcl_SetResult(interp,buffer,TCL_VOLATILE);
					break;
				case 12:
					cmsTakeColorants(&XYZtriple,profile);
					
					colorants[0]=Tcl_NewListObj(0,NULL);
					colorants[1]=Tcl_NewListObj(0,NULL);
					colorants[2]=Tcl_NewListObj(0,NULL);
					sprintf(buffer,"%.6f %.6f %.6f",XYZtriple.Red.X,XYZtriple.Red.Y,XYZtriple.Red.Z);
					Tcl_AppendToObj(colorants[0],buffer,strlen(buffer));
					sprintf(buffer,"%.6f %.6f %.6f",XYZtriple.Green.X,XYZtriple.Green.Y,XYZtriple.Green.Z);
					Tcl_AppendToObj(colorants[1],buffer,strlen(buffer));
					sprintf(buffer,"%.6f %.6f %.6f",XYZtriple.Blue.X,XYZtriple.Blue.Y,XYZtriple.Blue.Z);
					Tcl_AppendToObj(colorants[2],buffer,strlen(buffer));
					list=Tcl_NewListObj(3,colorants);
					Tcl_SetObjResult(interp,list);
					break;
				default:
					//blabla
					Tcl_SetResult(interp,"blabla",TCL_STATIC);
				}
			
			
			
			break;
	
				
		case 2:
		
			// cycle sets...
			if (Tcl_GetIndexFromObj(interp,objv[2],set_switches,"option",(int)NULL,&command_idx)==TCL_ERROR) return TCL_ERROR;
			//"pcs","colorspace","deviceclass","renderingintent","tag"
			switch (command_idx) {
				case 0:
					// pcs
					if (objc!=4) {
						Tcl_WrongNumArgs(interp,0,objv,"set pcs colorspaceid");
						return TCL_ERROR;
					}
					if (Tcl_GetIndexFromObj(interp,objv[3],colorspace_types,"deviceclass",(int)NULL,&idx)==TCL_ERROR) return TCL_ERROR;
					cmsSetPCS(profile,colorspace_typevals[idx]);
					break;
				case 1:
					if (objc!=4) {
						Tcl_WrongNumArgs(interp,0,objv,"set colorspace colorspaceid");
						return TCL_ERROR;
					}
					if (Tcl_GetIndexFromObj(interp,objv[3],colorspace_types,"deviceclass",(int)NULL,&idx)==TCL_ERROR) return TCL_ERROR;
					cmsSetColorSpace(profile,colorspace_typevals[idx]);

					break;
				case 2:
					// deviceclass
					if (objc!=4) {
						Tcl_WrongNumArgs(interp,0,objv,"set deviceclass deviceclassid");
						return TCL_ERROR;
					}

					if (Tcl_GetIndexFromObj(interp,objv[3],deviceclass_types,"deviceclass",(int)NULL,&idx)==TCL_ERROR) return TCL_ERROR;
					cmsSetDeviceClass(profile,deviceclass_typevals[idx]);
					break;
				case 3:
					// rendering intent
					if (objc!=4) {
						Tcl_WrongNumArgs(interp,0,objv,"set renderingintent 0|1|2|3");
						return TCL_ERROR;
					}
					
					intval=atoi(Tcl_GetString(objv[3]));
					cmsSetRenderingIntent(profile, intval);
					break;
				case 4:
					// Tags...
					break;
			}
			
			break;
		default:
			Tcl_SetResult(interp,"unknown option. use delete, get or set",TCL_STATIC);
			return TCL_ERROR;
			break;
	}



	return TCL_OK;

}

void css2result (Tcl_Interp *interp,icColorSpaceSignature css) {

	switch (css) {
	case icSigXYZData: Tcl_SetResult(interp,"XYZ ",TCL_STATIC);
		break;
	case icSigLabData: Tcl_SetResult(interp,"Lab ",TCL_STATIC);
		break;
	case icSigLuvData: Tcl_SetResult(interp,"Luv ",TCL_STATIC);
		break;
	case icSigYCbCrData: Tcl_SetResult(interp,"YCbr",TCL_STATIC);
		break;
	case icSigYxyData: Tcl_SetResult(interp,"Yxy ",TCL_STATIC);
		break;
	case icSigRgbData: Tcl_SetResult(interp,"RGB ",TCL_STATIC);
		break;
	case icSigGrayData: Tcl_SetResult(interp,"GRAY",TCL_STATIC);
		break;
	case icSigHsvData: Tcl_SetResult(interp,"HSV ",TCL_STATIC);
		break;
	case icSigHlsData: Tcl_SetResult(interp,"HLS ",TCL_STATIC);
		break;
	case icSigCmykData: Tcl_SetResult(interp,"CMYK",TCL_STATIC);
		break;
	case icSigCmyData: Tcl_SetResult(interp,"CMY ",TCL_STATIC);
		break;
	case icSig2colorData: Tcl_SetResult(interp,"2CLR",TCL_STATIC);
		break;
	case icSig3colorData: Tcl_SetResult(interp,"3CLR",TCL_STATIC);
		break;
	case icSig4colorData: Tcl_SetResult(interp,"4CLR",TCL_STATIC);
		break;
	case icSig5colorData: Tcl_SetResult(interp,"5CLR",TCL_STATIC);
		break;
	case icSig6colorData: Tcl_SetResult(interp,"6CLR",TCL_STATIC);
		break;
	case icSig7colorData: Tcl_SetResult(interp,"7CLR",TCL_STATIC);
		break;
	case icSig8colorData: Tcl_SetResult(interp,"8CLR",TCL_STATIC);
		break;
	case icSig9colorData: Tcl_SetResult(interp,"9CLR",TCL_STATIC);
		break;
	case icSig10colorData: Tcl_SetResult(interp,"ACLR",TCL_STATIC);
		break;
	case icSig11colorData: Tcl_SetResult(interp,"BCLR",TCL_STATIC);
		break;
	case icSig12colorData: Tcl_SetResult(interp,"CCLR",TCL_STATIC);
		break;
	case icSig13colorData: Tcl_SetResult(interp,"DCLR",TCL_STATIC);
		break;
	case icSig14colorData: Tcl_SetResult(interp,"ECLR",TCL_STATIC);
		break;
	case icSig15colorData: Tcl_SetResult(interp,"FCLR",TCL_STATIC);
		break;
	default: Tcl_SetResult(interp,"unknown",TCL_STATIC);
		break;
	}

return;
}


void deviceclass2result(Tcl_Interp *interp,icProfileClassSignature css) {


	switch (css) {

		case icSigInputClass: Tcl_SetResult(interp,"input",TCL_STATIC);break;
		case icSigOutputClass: Tcl_SetResult(interp,"output",TCL_STATIC);break;
		case icSigLinkClass: Tcl_SetResult(interp,"devicelink",TCL_STATIC);break;
		case icSigAbstractClass: Tcl_SetResult(interp,"abstract",TCL_STATIC);break;
		case icSigColorSpaceClass: Tcl_SetResult(interp,"colorspace",TCL_STATIC);break;
		case icSigNamedColorClass: Tcl_SetResult(interp,"named",TCL_STATIC);break;
		default: Tcl_SetResult(interp,"unknown",TCL_STATIC);break;
	}
	return;

}
