/***********************************************************************
* Little Colormanagement System - tcl-extension
* v0.20 (or just around that)
* 2004 Christian Bednarek
* ----------------------------------------------------------------------
* here you'll find the commands for handling and doing transformations
*
************************************************************************/

#include "tcllcms.h"
#include "tk.h"

void normalizeOutput(WORD Encoded[],int size,double normalizer) {
	int i;
	for (i=0;i<size;i++) {
		Encoded[i]=(int) floor(Encoded[i]/normalizer+0.5);
	}
}

void normalizeInput(WORD Encoded[],int size,double normalizer) {
	int i;	
	for (i=0;i<size;i++) {
		Encoded[i]=(int) floor(Encoded[i]*normalizer+0.5);
	}
}

int checkLengthList (Tcl_Interp *interp,int listc, int needed) {
	char buffer[255];
	if (listc!=needed) {
		sprintf(buffer,"input list must consist of %d elements, %d elements found",needed,listc);
		Tcl_SetResult(interp,buffer,TCL_VOLATILE);
		return TCL_ERROR;
	} else {
		return TCL_OK;
	}
}

DEFINE_CMD_E(tcl_cmsSetCMYKNormalized) {

	char buffer[10];
	char * onoff[]={"0","off","no","1","on","yes",(char *)NULL};
	int onoff_values[]={0,0,0,1,1,1,(int) NULL},idx;

	if (objc==1) {
		sprintf(buffer,"%d",cmyk_normalized);
		Tcl_SetResult(interp,buffer,TCL_VOLATILE);
		return TCL_OK;
	}

	if (objc!=2) {
		Tcl_WrongNumArgs(interp,1,objv,"0 or 1");
		return TCL_ERROR;
	}
	
	if (Tcl_GetIndexFromObj(interp,objv[1],onoff,"value",(int)NULL,&idx)==TCL_ERROR) return TCL_ERROR;
	cmyk_normalized=(int) onoff_values[idx];
	
	sprintf(buffer,"%d",cmyk_normalized);
	Tcl_SetResult(interp,buffer,TCL_VOLATILE);
	return TCL_OK;
	
}

DEFINE_CMD_E(ObjTransform) {

    char *command_list[]={"delete","get",(char *)NULL},buffer[255];
	char *get_list[]={"intent","proofingintent","type","inputprofile","outputprofile","proofingprofile","profiles",(char *)NULL};
	
	transform_data *trans;
	int idx_command,idx_get;


	trans=(transform_data *)clientData;

	if (objc==1) {
		Tcl_WrongNumArgs(interp,1,objv,"delete or get");
		return TCL_ERROR;
	}
	
	if (Tcl_GetIndexFromObj(interp,objv[1],command_list,"command",(int)NULL,&idx_command)!=TCL_OK) return TCL_ERROR;
	
	switch (idx_command) {
	case 0:
		Tcl_DeleteCommand(interp,Tcl_GetString(objv[0]));
		break;
    
	case 1:
		{
		
			if (objc==2) {
				Tcl_WrongNumArgs(interp,2,objv,"option");
				return TCL_ERROR;
			}

			if (Tcl_GetIndexFromObj(interp,objv[2],get_list,"option",(int)NULL,&idx_get)!=TCL_OK) return TCL_ERROR;
			switch (idx_get) {
			case 0:
				sprintf(buffer,"%d",trans->intent);
				break;
			case 1:
				sprintf(buffer,"%d",trans->proofingintent);
				break;
			case 2:
				if (trans->proofingtrans) sprintf(buffer,"proofingtransform"); else sprintf(buffer,"transform");
				break;
			case 3:
				sprintf(buffer,trans->input_handle);
				break;
			case 4:
				sprintf(buffer,trans->output_handle);
				break;
			case 5:
				if (trans->proofingtrans==TRUE) sprintf(buffer,trans->proofing_handle); else sprintf(buffer,"");
				break;
			case 6:
				if (trans->proofingtrans==TRUE) sprintf(buffer,"%s %s %s",trans->input_handle,trans->output_handle,trans->proofing_handle); else
					sprintf(buffer,"%s %s",trans->input_handle,trans->output_handle);
				break;

			}

		}
	}
	
	Tcl_SetResult(interp,buffer,TCL_VOLATILE);
	return TCL_OK;

}

extern void ObjTransformDelete (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {

   transform_data *vv;
   vv = (transform_data *) clientData;	 
   cmsDeleteTransform( vv->transform );
   free(vv);
  

}


extern ClientData getDataFromObj (Tcl_Interp *interp, char *cmdname) {
    Tcl_CmdInfo info;
	Tcl_GetCommandInfo(interp, cmdname, &info);
	return info.objClientData;
}


extern cmsHTRANSFORM getTransFromObj (Tcl_Interp *interp, char *cmdname) {

	transform_data *vv;

    Tcl_CmdInfo info;
	Tcl_GetCommandInfo(interp, cmdname, &info);
	vv = (transform_data *) info.objClientData;
	return (vv->transform);

}

extern cmsHPROFILE getTransProfile (Tcl_Interp *interp, char *cmdname,int inout) {

	transform_data *vv;
    Tcl_CmdInfo info;

	Tcl_GetCommandInfo(interp, cmdname, &info);
	vv = (transform_data *) info.objClientData;
	if (inout==1) {
		return (vv->input);
	} else {
		return (vv->output);
	}
}



DEFINE_CMD_E(tcl_cmsCreateTransform) {


	cmsHTRANSFORM trans;
	cmsHPROFILE mProfiles[16],input,output,proof=NULL;
	transform_data *tdata;
	
	char * commands[]={"cmsCreateTransform","cmsCreateProofingTransform","cmsCreateMultiprofileTransform",(char *) NULL};
	char * flagnames[]={
						"-matrixinput","-matrixoutput","-matrixonly","-notprecalc",
						"-nulltranform","-highresprecalc","-lowresprecalc",
						"-blackpointcompensation",(char *)NULL
	};
	
	int command_flags[]={
						 cmsFLAGS_MATRIXINPUT,cmsFLAGS_MATRIXOUTPUT,cmsFLAGS_MATRIXONLY,
						 cmsFLAGS_NOTPRECALC,cmsFLAGS_NULLTRANSFORM,cmsFLAGS_HIGHRESPRECALC,
						 cmsFLAGS_LOWRESPRECALC,cmsFLAGS_BLACKPOINTCOMPENSATION,(int)NULL
	};
	
	int plistc,idx,type,i,flags,intent,proofingintent,options_start=0;
	Tcl_Obj **plist;
	char buffer[100];    
    DWORD dwIn, dwOut;

	if (Tcl_GetIndexFromObj(interp,objv[0],commands,"command",(int)NULL,&idx)==TCL_ERROR) return TCL_ERROR;

	type=idx;
	flags=0;

	switch (type) {
	case 0:
		if (objc<4) { 
			Tcl_WrongNumArgs(interp,objc,objv,"inputProfile outputProfile intent ?option?");
			return TCL_ERROR;
		}
		
		if (objc>4) {
			options_start=4;
		}

		intent=atoi(Tcl_GetString(objv[3]));
		proofingintent=-1;
		break;

	case 1:

		if (objc<6) { 
			Tcl_WrongNumArgs(interp,objc,objv,"inputProfile outputProfile proofingProfile intent proofingintent ?option?");
			return TCL_ERROR;
		}
		
		if (objc>6) {
			options_start=6;
		}
		
		intent=atoi(Tcl_GetString(objv[4]));
		proofingintent=atoi(Tcl_GetString(objv[5]));
		break;
	

	case 2:
	
		if (objc<2) {
			Tcl_WrongNumArgs(interp,objc,objv,"profilelist intent ?option?");
			return TCL_ERROR;
		}
		
		if (objc>3) {
			options_start=3;
		}

		intent=atoi(Tcl_GetString(objv[2]));
		proofingintent=-1;
		break;
	}

	// handling options...
	
	for (i=options_start;i<objc && i!=0;i++) {
		if (Tcl_GetIndexFromObj(interp,objv[i],flagnames,"option",(int)NULL,&idx)==TCL_ERROR) return TCL_ERROR;
		flags=flags|command_flags[idx];
	}
	
	if (type==1) {
		flags=flags|cmsFLAGS_SOFTPROOFING;
	}
	if (type!=2) {
		input=getProfileFromObj(interp,Tcl_GetString(objv[1]));
		output=getProfileFromObj(interp,Tcl_GetString(objv[2]));
	
		
	} else {
		// multiple transform
		
		if (Tcl_ListObjGetElements(interp, objv[1], &plistc, &plist)==TCL_ERROR) return TCL_ERROR;
		for (i=0;i<plistc;i++) {
			mProfiles[i]=getDataFromObj(interp,Tcl_GetString(plist[i]));
		}
		input=mProfiles[0];
		output=mProfiles[plistc-1];
		
	}

	dwIn = BYTES_SH(2) | CHANNELS_SH(_cmsChannelsOf(cmsGetColorSpace(input)));
	dwOut = BYTES_SH(2) | CHANNELS_SH(_cmsChannelsOf(cmsGetColorSpace(output)));	

	if (type==1) {
		proof=getProfileFromObj(interp,Tcl_GetString(objv[3]));
	}
   
	sprintf(buffer,"trans%d",lasttransnum);
	lasttransnum++;

	if (type==0) {
		trans=cmsCreateTransform(input,dwIn,output, dwOut,intent,flags);
    } else if (type==1) {
		trans=cmsCreateProofingTransform(input,dwIn,output, dwOut, proof, intent,proofingintent,flags|cmsFLAGS_SOFTPROOFING);
	} else {
		trans=cmsCreateMultiprofileTransform(mProfiles,plistc,dwIn,dwOut,intent,flags);
	}
 
	if (trans==NULL) {
		Tcl_SetResult(interp,"error creating transformation",TCL_STATIC);
		return TCL_ERROR;
	}

	tdata =  malloc (sizeof(transform_data)+8);
	
	tdata->input=input;
	tdata->output=output;
	tdata->proof=proof;
	tdata->transform=trans;
	tdata->intent=intent;
	tdata->proofingintent=proofingintent;

	tdata->input_handle=Tcl_GetString(objv[1]);
	tdata->output_handle=Tcl_GetString(objv[2]);
	if (type==1) {
		tdata->proofing_handle=Tcl_GetString(objv[3]);
		tdata->proofingtrans=TRUE;
	} else {
		tdata->proofing_handle=(char *)NULL;
		tdata->proofingtrans=FALSE;
	}

	Tcl_CreateObjCommand(interp,(char *)&buffer,ObjTransform,tdata,(Tcl_CmdDeleteProc *)&ObjTransformDelete);
    Tcl_SetResult(interp,(char *)&buffer,TCL_VOLATILE); 

  
	return TCL_OK;
  
}

int getInputFromList (Tcl_Interp *interp,WORD input[], Tcl_Obj **list, int listc, icColorSpaceSignature iPS)

{
	cmsCIEXYZ xyz;
	cmsCIELab Lab;
	double norm;
	
	switch (iPS) {
	
	case icSigXYZData:
					if (checkLengthList(interp,listc,3)==TCL_ERROR) return TCL_ERROR;
					getXYZFromList(&xyz,list);
					cmsFloat2XYZEncoded(input, &xyz);                 
                    break;

    case icSigLabData:
					if (checkLengthList(interp,listc,3)==TCL_ERROR) return TCL_ERROR;
    				getLabFromList(&Lab,list);
					cmsFloat2LabEncoded(input, &Lab);
                    break;

    case icSigLuvData:
    case icSigYCbCrData:
    case icSigYxyData:
    case icSigRgbData:
    case icSigHsvData:
	case icSigHlsData:
                    if (checkLengthList(interp,listc,3)==TCL_ERROR) return TCL_ERROR;
					getValFromList(input,list,3,257);
	                break;
    case icSigCmyData:                        
					if (checkLengthList(interp,listc,3)==TCL_ERROR) return TCL_ERROR;
					if (cmyk_normalized==1) norm=655.35; else norm=257;
					getValFromList(input,list,3,norm);
					break;
    case icSigGrayData:
                    if (checkLengthList(interp,listc,1)==TCL_ERROR) return TCL_ERROR;
					getValFromList(input,list,1,257);
	                break;
    case icSigCmykData:
                    if (checkLengthList(interp,listc,4)==TCL_ERROR) return TCL_ERROR;
					if (cmyk_normalized==1) norm=655.35; else norm=257;
					getValFromList(input,list,4,norm);
					break;
    case icSig6colorData:
					if (checkLengthList(interp,listc,6)==TCL_ERROR) return TCL_ERROR;
					getValFromList(input,list,6,257);
	                break;
    default:
					Tcl_SetResult(interp,"unsupported colorspace",TCL_STATIC);
					return TCL_ERROR;
    }
	
	return TCL_OK;
}


char * setOutputResult(Tcl_Interp *interp,WORD Encoded[],icColorSpaceSignature oPS) {
	
	
	char * buffer;
	cmsCIEXYZ xyz;
	cmsCIELab Lab;
	double norm;
	buffer=malloc(255);

	switch (oPS) {

		
    case icSigXYZData:
                    cmsXYZEncoded2Float(&xyz, Encoded);
                    sprintf(buffer,"%.6f %.6f %.6f",xyz.X,xyz.Y,xyz.Z);
                    break;

    case icSigLabData:
                    cmsLabEncoded2Float(&Lab, Encoded);
                    sprintf(buffer,"%.6f %.6f %.6f",Lab.L,Lab.a,Lab.b);
                    break;

    case icSigLuvData:
    case icSigYCbCrData:
    case icSigYxyData:
    case icSigRgbData:
    case icSigHsvData:
    case icSigHlsData:
					normalizeOutput(Encoded,3,257);
					sprintf(buffer,"%d %d %d",Encoded[0],Encoded[1],Encoded[2]);
	                break;
    case icSigGrayData:
					normalizeOutput(Encoded,1,257);
					sprintf(buffer,"%d",Encoded[0]);
	                break;
	case icSigCmyData:                        
					if (cmyk_normalized==1) norm=655.35; else norm=257;
					normalizeOutput(Encoded,3,norm);
					sprintf(buffer,"%d %d %d",Encoded[0],Encoded[1],Encoded[2]);
					break;
    case icSigCmykData:
					if (cmyk_normalized==1) norm=655.35; else norm=257;
					normalizeOutput(Encoded,4,norm);
					sprintf(buffer,"%d %d %d %d",Encoded[0],Encoded[1],Encoded[2],Encoded[3]);
					break;

    case icSig6colorData:
                    normalizeOutput(Encoded,6,257);
					sprintf(buffer,"%d %d %d %d %d %d",Encoded[0],Encoded[1],Encoded[2],Encoded[3],Encoded[4],Encoded[5]);
	                break;
    default:
					sprintf(buffer,"unknown output colorspace?");
					break;
	}

	return buffer;
}



DEFINE_CMD_E(tcl_cmsDoTransform) {


	cmsHTRANSFORM trans;
	cmsHPROFILE iP,oP;
    icColorSpaceSignature iPS,oPS;
	Tcl_Obj **list;
	int listc;
	char * result_string;
	WORD input[MAXCHANNELS], output[MAXCHANNELS];
    
    if (objc!=3) { 
      Tcl_WrongNumArgs(interp,objc,objv,"trans input");
      return TCL_ERROR;
	}
    	
	iP=getTransProfile(interp,Tcl_GetString(objv[1]),1);
	oP=getTransProfile(interp,Tcl_GetString(objv[1]),2);

	iPS=cmsGetColorSpace(iP);
	oPS=cmsGetColorSpace(oP);

	trans=getTransFromObj(interp,Tcl_GetString(objv[1]));
    
	if (Tcl_ListObjGetElements(interp, objv[2], &listc, &list)==TCL_ERROR) return TCL_ERROR;
	if (getInputFromList (interp,input,list,listc,iPS)==TCL_ERROR) return TCL_ERROR;
	cmsDoTransform(trans, input, output, 1);
	
	result_string=setOutputResult(interp,output,oPS);
	Tcl_SetResult(interp,result_string,TCL_VOLATILE);
	free(result_string);

	return TCL_OK;
}


DEFINE_CMD_E(tcl_cmsTransformImage) {


	cmsHTRANSFORM trans;
	cmsHPROFILE iP,oP;
    icColorSpaceSignature iPS,oPS;
	
	Tk_PhotoHandle handle;
	Tk_PhotoImageBlock blockPtr;
    int i;
    register int x,y;
 	WORD *tbuffer,*destPtr;
	

    if (objc!=3) { 
      Tcl_WrongNumArgs(interp,objc,objv,"trans image");
      return TCL_ERROR;
	}
    
	handle=Tk_FindPhoto(interp,Tcl_GetString(objv[2]));
	Tk_PhotoGetImage(handle, &blockPtr);

	iP=getTransProfile(interp,Tcl_GetString(objv[1]),1);
	oP=getTransProfile(interp,Tcl_GetString(objv[1]),2);

	iPS=cmsGetColorSpace(iP);
	oPS=cmsGetColorSpace(oP);

	trans=getTransFromObj(interp,Tcl_GetString(objv[1]));
	
	destPtr=malloc(blockPtr.pitch*4);
	tbuffer=malloc(blockPtr.pitch*4);



    for (y = 0; y < blockPtr.height; y++) {
		
		for (x = 0 ; x <blockPtr.width; x++) {
			for (i=0 ; i < 3; i++) {
				tbuffer[x*3+i]=blockPtr.pixelPtr[y*blockPtr.pitch+(x*4+i)]/255.0*65535.0;
			}
			
		}	
	
		cmsDoTransform(trans,tbuffer,destPtr,blockPtr.width);
		
		for (x = 0 ; x <blockPtr.width; x++) {
			for (i=0 ; i < 3; i++) {
				blockPtr.pixelPtr[y*blockPtr.pitch+(x*4+i)]=destPtr[x*3+i]*255.0/65535.0;
					
			}
		}
		
	}

				
	free(destPtr);
	free(tbuffer);
	
	return TCL_OK;
}

