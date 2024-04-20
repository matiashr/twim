FUNCTION_BLOCK FB_SystemInfo EXTENDS Mycronic.Task IMPLEMENTS ITF_SystemInfo, Mycronic.ITF_InstanceConfig
VAR
	///FB interface version
	{attribute 'OPC.UA.DA.Access' := '1'}
	{attribute 'OPC.UA.DA' := '1'}
	InterfaceVersion: STRING;
	///Number of entries to read. Indexing starts at 0
	{attribute 'OPC.UA.DA.Access' := '1'}
	{attribute 'OPC.UA.DA' := '1'}
	NoEntries: UINT:=3;
	{attribute 'OPC.UA.DA' := '0'}
	instance: E_INSTANCES;
	versions : ARRAY [0..300] OF S_VersionInfo;
	ErrorCode: E_SDOErrors;
	buf: ARRAY [0..20] OF BYTE;
	fbformat:Tc2_Utilities.FB_FormatString;
	i: UINT;
	doLogStartup: BOOL;
	hexStr2Dword : FB_HEXSTR_TO_DWORD;
	slaveAddrD: DWORD;
	slaveAddr: UINT;
	liftSlaveAddr: UINT;
	rotSlaveAddr: UINT;
	doorSlaveAddr: UINT;
END_VAR
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR
VAR CONSTANT
END_VAR
VAR
	{attribute 'OPC.UA.DA' := '0'}
	//sdoreader:FB_SDOReadWrite;
	completed:BOOL:=FALSE;
	state:INT:=0;
	lifterVer:STRING:='Not available';
	rotatorVer:STRING:='Not available';
	lifterConf:STRING:='Not available';
	rotatorConf:STRING:='Not available';
	carrierVer:STRING:='Not available';
	loadDoorVerSw:STRING:='Not available';
	loadDoorVerHw:STRING:='Not available';
	pos:UINT;	
	entry:STRING;
END_VAR
VAR
	{attribute 'OPC.UA.DA' := '0'}
	status: E_PARAMSTATUS;
	{attribute 'OPC.UA.DA' := '0'}
	uparameters: U_Parameters;
	{attribute 'OPC.UA.DA' := '0'}
	tmp:UDINT;
	{attribute 'OPC.UA.DA' := '0'}
	stat:Mycronic.E_FunctionState;
	{attribute 'OPC.UA.DA' := '0'}
	errCode: E_SDOErrors;
	{attribute 'OPC.UA.DA' := '0'}
	runtime:FB_GetTwincatRealHW;
END_VAR

      (* 
	Req/Real hw alike info, according to https://confluence.miclaser.net/display/JPS/ReqHW+Required+Hardware 

	Technosoft:
		Index:1009 = The object contains the manufacturer hardware version in ASCII form, maximum 15 characters
	
	Nidec: 
		Index:0x1018 Identity object
		Sub-index 2
			Access: RO Range: N/A Size: 4 bytes Unit: N/A
			Default: 0x01mmvvtt Type: UDINT
			Description:
			Product code.
			This contains the drive product code.
			Byte0 (tt): Drive type (2 =M753)
			Byte1 (vv): Drive variant/derivative (derived from Pr 11.028)
			Byte2 (mm): Drive mode (derived from Pr 11.084)
			Byte3 (gg): Drive generation (0 = Unidrive SP; 1 = Unidrive M)			
		Sub-index 3
			Access: RO Range: N/A Size: 4 bytes Unit: N/A
			Default: Derived from Pr S.00.002 Type: UDINT
			Description: Module firmware version in the format major.minor.version.build		
*)

(* if done then do nothing *)
IF completed THEN
	RETURN;
END_IF

(* read all revisions *)
CASE state OF	
	0:	(* startup *)
		sdo.sdoRead(bExecute:=FALSE);
		state:=state+1;
		hexStr2Dword(HexString := diagnostics.ecatNodeAddress[E_ECNodeNames.A5A7], Data => slaveAddrD);
		liftSlaveAddr:=DWORD_TO_UINT(slaveAddrD);
		hexStr2Dword(HexString := diagnostics.ecatNodeAddress[E_ECNodeNames.A5A6], Data => slaveAddrD);
		rotSlaveAddr:=DWORD_TO_UINT(slaveAddrD);		
		
		hexStr2Dword(HexString := diagnostics.ecatNodeAddress[E_ECNodeNames.A6A6], Data => slaveAddrD);
		doorSlaveAddr:=DWORD_TO_UINT(slaveAddrD);				
		
	1:	(* read fw version of lifter *)
						
		sdo.sdoRead(sNetId:=system.EtherCatAmsNetId, nSlaveAddr:=liftSlaveAddr,nIndex:=16#100A, nSubIndex:=0,pDstBuf:=ADR(buf),cbBufLen:=15,bExecute:=TRUE);
		IF NOT(sdo.sdoRead.bBusy) THEN
			IF NOT(sdo.sdoRead.bError) THEN
				fbformat(sFormat:='%c%c%c%c%c',	arg1:=Tc2_Utilities.F_BYTE(buf[0]), 
												arg2:=Tc2_Utilities.F_BYTE(buf[1]),
												arg3:=Tc2_Utilities.F_BYTE(buf[2]),
												arg4:=Tc2_Utilities.F_BYTE(buf[3]),								
												arg5:=Tc2_Utilities.F_BYTE(buf[4]),
												sOut=>lifterVer													
				);
			ELSE
				logmem.MyLogger(level:=E_LogLevel.ERROR,format:='Lifter version not available');			
			END_IF
			sdo.sdoRead(bExecute:=FALSE);
			state:=state+1;
		END_IF	
	2:	(* read fw version of rotator drive *)		
		sdo.sdoRead(sNetId:=system.EtherCatAmsNetId, nSlaveAddr:=rotSlaveAddr,nIndex:=16#100A, nSubIndex:=0,pDstBuf:=ADR(buf),cbBufLen:=15,bExecute:=TRUE);
		IF NOT(sdo.sdoRead.bBusy) THEN
			IF NOT(sdo.sdoRead.bError) THEN
				fbformat(sFormat:='%c%c%c%c%c',	arg1:=Tc2_Utilities.F_BYTE(buf[0]), 
												arg2:=Tc2_Utilities.F_BYTE(buf[1]),
												arg3:=Tc2_Utilities.F_BYTE(buf[2]),
												arg4:=Tc2_Utilities.F_BYTE(buf[3]),								
												arg5:=Tc2_Utilities.F_BYTE(buf[4]),
												sOut=>rotatorVer													
				);
			ELSE
				logmem.MyLogger(level:=E_LogLevel.ERROR,format:='Rotator version not available');
			END_IF
			sdo.sdoRead(bExecute:=FALSE);
			state:=state+1;
		END_IF
	3:	(* read carrier fw version *)
		sdo.sdoRead(sNetId:=system.EtherCatAmsNetId, nSlaveAddr:=1014,nIndex:=16#1018, nSubIndex:=3,pDstBuf:=ADR(buf),cbBufLen:=4,bExecute:=TRUE);
		IF NOT(sdo.sdoRead.bBusy) THEN
			IF NOT(sdo.sdoRead.bError) THEN
				fbformat(sFormat:='%d.%d.%d.%d',	arg1:=Tc2_Utilities.F_BYTE(buf[0]), 
												arg2:=Tc2_Utilities.F_BYTE(buf[1]),
												arg3:=Tc2_Utilities.F_BYTE(buf[2]),
												arg4:=Tc2_Utilities.F_BYTE(buf[3]),								
												sOut=>carrierVer													
				);
			ELSE
				logmem.MyLogger(level:=E_LogLevel.ERROR,format:='Carrier version not available');
			END_IF
			sdo.sdoRead(bExecute:=FALSE);
			state:=state+1;
		END_IF
	4:	(* read config version for lifter *)
		sdo.sdoRead(sNetId:=system.EtherCatAmsNetId, nSlaveAddr:=liftSlaveAddr,nIndex:=16#2060, nSubIndex:=0,pDstBuf:=ADR(buf),cbBufLen:=15,bExecute:=TRUE);
		IF NOT(sdo.sdoRead.bBusy) THEN
			IF NOT(sdo.sdoRead.bError) THEN
				fbformat(sFormat:='%c%c%c%c',	arg1:=Tc2_Utilities.F_BYTE(buf[0]), 
												arg2:=Tc2_Utilities.F_BYTE(buf[1]),
												arg3:=Tc2_Utilities.F_BYTE(buf[2]),
												arg4:=Tc2_Utilities.F_BYTE(buf[3]),								
												sOut=>lifterConf													
				);
			ELSE
				logmem.MyLogger(level:=E_LogLevel.ERROR,format:='Lifter config not available');			
			END_IF
			sdo.sdoRead(bExecute:=FALSE);
			state:=state+1;
		END_IF	
	5:	(* read config version for rotator *)
		sdo.sdoRead(sNetId:=system.EtherCatAmsNetId, nSlaveAddr:=rotSlaveAddr,nIndex:=16#2060, nSubIndex:=0,pDstBuf:=ADR(buf),cbBufLen:=15,bExecute:=TRUE);
		IF NOT(sdo.sdoRead.bBusy) THEN
			IF NOT(sdo.sdoRead.bError) THEN
				fbformat(sFormat:='%c%c%c%c',	arg1:=Tc2_Utilities.F_BYTE(buf[0]), 
												arg2:=Tc2_Utilities.F_BYTE(buf[1]),
												arg3:=Tc2_Utilities.F_BYTE(buf[2]),
												arg4:=Tc2_Utilities.F_BYTE(buf[3]),								
												sOut=>rotatorConf													
				);
			ELSE
				logmem.MyLogger(level:=E_LogLevel.ERROR,format:='Rotator config not available');			
			END_IF
			sdo.sdoRead(bExecute:=FALSE);
			state:=state+1;
		END_IF				
	6:	(* read runtime version data *)
		runtime(MasterAmsNetId:=system.MasterAmsNetId);
		IF runtime.Done THEN
			logmem.MyLogger(level:=E_LogLevel.NORMAL,format:='read runtime versions');
 			state:=state+1;
		END_IF
	7:
		sdo.sdoRead(sNetId:=system.EtherCatAmsNetId, nSlaveAddr:=doorSlaveAddr,nIndex:=16#1009, nSubIndex:=0,pDstBuf:=ADR(buf),cbBufLen:=15,bExecute:=TRUE);
		IF NOT(sdo.sdoRead.bBusy) THEN
			IF NOT(sdo.sdoRead.bError) THEN
				fbformat(sFormat:='%c%c',	arg1:=Tc2_Utilities.F_BYTE(buf[0]), arg2:=Tc2_Utilities.F_BYTE(buf[1]), sOut=>loadDoorVerHw );
			ELSE
				logmem.MyLogger(level:=E_LogLevel.ERROR,format:='Loaderdoor drive hw not available');			
			END_IF
			sdo.sdoRead(bExecute:=FALSE);
			state:=state+1;
		END_IF
	8:
		sdo.sdoRead(sNetId:=system.EtherCatAmsNetId, nSlaveAddr:=doorSlaveAddr,nIndex:=16#100A, nSubIndex:=0,pDstBuf:=ADR(buf),cbBufLen:=15,bExecute:=TRUE);
		IF NOT(sdo.sdoRead.bBusy) THEN
			IF NOT(sdo.sdoRead.bError) THEN
				fbformat(sFormat:='%c%c',	arg1:=Tc2_Utilities.F_BYTE(buf[0]), arg2:=Tc2_Utilities.F_BYTE(buf[1]), sOut=>loadDoorVerSw );
			ELSE
				logmem.MyLogger(level:=E_LogLevel.ERROR,format:='Loaderdoor drive sw not available');			
			END_IF
			sdo.sdoRead(bExecute:=FALSE);
			state:=state+1;
		END_IF
	9:	(* completed *)
		completed:=TRUE;
END_CASE

IF completed THEN
	THIS^.versions[pos].Tag:='Glc.FwVersion';
	THIS^.versions[pos].Version:=info.FwVersion;
	THIS^.versions[pos].Comment:='//GLC fw version';
	pos:=pos+1;
	
	THIS^.versions[pos].Tag:='RU.Rotator.drivefw';
	THIS^.versions[pos].Version:=THIS^.rotatorVer;
	THIS^.versions[pos].Comment:='//TechnoSoft drive fw';
	pos:=pos+1;
	
	THIS^.versions[pos].Tag:='RU.Rotator.driveconf';
	THIS^.versions[pos].Version:=THIS^.rotatorConf;
	THIS^.versions[pos].Comment:='//TechnoSoft drive config';	
	pos:=pos+1;
	
	THIS^.versions[pos].Tag:='RU.Lifter.drivefw';
	THIS^.versions[pos].Version:=THIS^.lifterVer;
	THIS^.versions[pos].Comment:='//TechnoSoft drive fw';
	pos:=pos+1;
	
	THIS^.versions[pos].Tag:='RU.Lifter.driveconf';
	THIS^.versions[pos].Version:=THIS^.lifterConf;
	THIS^.versions[pos].Comment:='//TechnoSoft drive config';
	pos:=pos+1;
		
	THIS^.versions[pos].Tag:='Glc.Carrier';
	THIS^.versions[pos].Version:=THIS^.carrierVer;
	THIS^.versions[pos].Comment:='//Nidec M753 RFC-S drive fw';
	pos:=pos+1;
	
	THIS^.versions[pos].Tag:='Glc.LoadDoorHW';
	THIS^.versions[pos].Version:=THIS^.loadDoorVerSw;
	THIS^.versions[pos].Comment:='//EL7342 hw';
	pos:=pos+1;	
	
	THIS^.versions[pos].Tag:='Glc.LoadDoorSw';
	THIS^.versions[pos].Version:=THIS^.loadDoorVerHw;
	THIS^.versions[pos].Comment:='//EL7342 sw';
	pos:=pos+1;		
		
	(* EAP version *)
	THIS^.versions[pos].Tag:='GLCEapVersion';
	THIS^.versions[pos].Version:=EAPdata.EAPversion;
	THIS^.versions[pos].Comment:='//EAP interface version';
	pos:=pos+1;	
	
	FOR i:=0 TO INT_TO_UINT(runtime.NoEntries) DO
		IF pos+i < INT_TO_UINT(SIZEOF(versions)) THEN
			THIS^.versions[pos].Tag:=concat('PLC.', THIS^.runtime.Entries[i]);		//Entries contains a fully formated entry, so set version,comment to ''
			THIS^.versions[pos].Version:='';
			THIS^.versions[pos].Comment:='//PLC runtime info';
			pos:=pos+1;
		END_IF
	END_FOR
		
	NoEntries:=pos;
ELSE
	NoEntries:=0;
END_IF

(* log everything if requested at completion *)
IF completed AND doLogStartup THEN
	FOR i:=0 TO THIS^.NoEntries DO
		// versions[EntryNo].Tag, versions[EntryNo].Version
		fbformat( sFormat:='%s %s %s', 	arg1:=Tc2_Utilities.F_STRING(versions[i].Tag), 
										arg2:=Tc2_Utilities.F_STRING(versions[i].Version), 
										arg3:=Tc2_Utilities.F_STRING(versions[i].Comment), sOut=>entry );
		logmem.MyLogger(level:=E_LogLevel.ERROR,format:=entry);		
	END_FOR
	doLogStartup:=FALSE;
END_IF

    

      METHOD configure : BOOL
VAR_INPUT
END_VAR
VAR_OUTPUT
	///status of configure
	ErrorCode: E_InstanceConfigErrors;
END_VAR

VAR
	done:BOOL;
END_VAR



      
        IF NOT(done) THEN	
	THIS^.register();			//Register task in scheduler
	StartupLog();				// log everything by default
	done:=TRUE;
END_IF
configure:=TRUE;
      
    

      METHOD getInstanceNo : UDINT
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR
VAR
END_VAR

      
        
			(* AUTOGENERATED do not modify *)
			getInstanceNo:=THIS^.instance;

      
    

      {attribute 'TcRpcEnable' := '1'}
METHOD GetSystemInfo
VAR_INPUT
	///Entry number to read. Indexing starts at 0
	EntryNo: UINT;
END_VAR
VAR_OUTPUT
	///System info entry
	Entry: STRING;
	///Error code [iserrorcode]
	ErrorCode: E_SystemInfoErrors;
END_VAR


      
        IF NOT(THIS^.completed) THEN
	ErrorCode:=E_SystemInfoErrors.ACQUIRING;
END_IF

IF EntryNo <=THIS^.NoEntries THEN
	logmem.MyLogger(level:=E_LogLevel.NORMAL,format:='system info requested');
	// versions[EntryNo].Tag, versions[EntryNo].Version
	fbformat( sFormat:='%s %s %s', 	arg1:=Tc2_Utilities.F_STRING(versions[EntryNo].Tag), 
									arg2:=Tc2_Utilities.F_STRING(versions[EntryNo].Version), 
									arg3:=Tc2_Utilities.F_STRING(versions[EntryNo].Comment), sOut=>Entry );
	ErrorCode:=E_SystemInfoErrors.NO_ERROR;
ELSE
	ErrorCode:=E_SystemInfoErrors.NO_ENTRY;
	logmem.MyLogger(level:=E_LogLevel.ERROR,format:='configure has not been able to read any versions');
END_IF

      
    

      METHOD myInit : BOOL
VAR_INPUT
	///Instance identifier
	instance: E_INSTANCES;
	///FB interface version
	InterfaceVersion: STRING;
END_VAR
VAR_OUTPUT
END_VAR
VAR
END_VAR

      
        THIS^.instance := instance;	(* instance identifier *)
THIS^.InterfaceVersion := InterfaceVersion; (* FB interface version *)

      
    

      METHOD postConfigure : BOOL
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR
VAR
END_VAR

      
        (*Autogenerate stub, NOTE: it's not allowed to perform anything that cannot finish within one PLC cycle here.*)
postConfigure:=True;

      
    

      METHOD preConfigure : BOOL
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR
VAR
END_VAR

      
        (*Autogenerate stub, NOTE: it's not allowed to perform anything that cannot finish within one PLC cycle here.*)
preConfigure:=True;

      
    

      METHOD register : BOOL
VAR_INPUT
END_VAR
VAR_OUTPUT
END_VAR
VAR
END_VAR

      
        m_scheduler:=ADR(evloop.evScheduler);
m_name:=ENUM_TAB.E_INSTANCES_STR[THIS^.instance];
m_runMode:=true;
m_cycletime:=T#0S;
m_timer(IN:=FALSE, PT:=m_cycletime);
m_scheduler^.registerTask( THIS );

      
    

      METHOD selfTest : BOOL
VAR_INPUT
END_VAR
VAR_OUTPUT
	///Status message of self test
	message: T_MaxString;
	///Status of self test
	status: E_SelfTestStatus;
END_VAR
VAR
END_VAR

      
        (* Default autogenerated Method BODY GOES HERE *)

      
    

      METHOD setHwDegraded : BOOL
VAR_INPUT
	status: BOOL;
END_VAR
VAR_OUTPUT
END_VAR
VAR
END_VAR

      
        (* Default autogenerated Method BODY GOES HERE *)

      
    

      METHOD StartupLog
VAR_INPUT
	
END_VAR

      
        doLogStartup:=true;
      
    
END_FUNCTION_BLOCK
