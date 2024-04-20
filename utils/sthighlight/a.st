// This functionblock acts as a stream buffer for use with FB_XmlControl
FUNCTION_BLOCK FB_StreamBuffer 
VAR
    _PointerToStringBuffer : POINTER TO BYTE;
    _BufferSize : UDINT;
    _Length : UDINT;
END_VAR

      
    
// Appends a string to the buffer
PROPERTY Append : T_MaxString
VAR
    ByteIn : POINTER TO BYTE;
    ByteBuffer : POINTER TO BYTE;
END_VAR

          ByteIn := ADR(Append); 
ByteBuffer := _PointerToStringBuffer + _Length; // set start address

WHILE ByteIn^ <> 0 AND (_Length < _BufferSize ) DO
    ByteBuffer^ := ByteIn^; //Copy the 1st byte
    _Length := _Length + 1; // set new buffer length
    ByteIn := ByteIn + 1; // calculate new start address 
    ByteBuffer := ByteBuffer + 1; // calculate new start address 
END_WHILE;

ByteBuffer := _PointerToStringBuffer + _Length; // String End
ByteBuffer^ := 0; // null terminated string
        
// Read current Buffersize
PROPERTY BufferSize : UDINT


          BufferSize := _BufferSize;
        
// Clears the buffer and sets the length to 0
METHOD PUBLIC Clear
VAR
	Count : UDINT;
END_VAR

        IF (_PointerToStringBuffer = 0) OR (_BufferSize = 0) THEN
    RETURN;
END_IF

FOR Count := 0 TO (_BufferSize - 1) DO
    _PointerToStringBuffer[Count] := 0;
END_FOR

_Length := 0;
      
// Copies a string from the character buffer
METHOD PUBLIC Copy : T_MaxString
VAR_INPUT
    StartPos : UDINT;
    EndPos : UDINT;
END_VAR
VAR_OUTPUT
    CopyLen : UDINT;
    XmlError : E_XmlError;
END_VAR
VAR
    Loop : UDINT;
    PointerToByteToCopy : POINTER TO BYTE;
    PointerToBuffer : POINTER TO BYTE;
    CurPos : UDINT;
END_VAR

Loop := 0;
PointerToByteToCopy := ADR(Copy);
PointerToBuffer := _PointerToStringBuffer + StartPos - 1;

WHILE(Loop < SIZEOF(Copy)) AND (StartPos - 1 + Loop < _Length) AND (StartPos + Loop < EndPos) DO
    PointerToByteToCopy^ := PointerToBuffer^;
    Loop := Loop + 1;
    PointerToByteToCopy := ADR(Copy) + Loop;
    PointerToBuffer := _PointerToStringBuffer + StartPos + Loop -1;
END_WHILE;

IF Loop = SIZEOF(Copy) THEN
    XmlError := E_XmlError.ErrorStringLen;
ELSIF StartPos - 1 + Loop = _Length THEN
    XmlError := E_XmlError.ErrorMaxBufferLen;
ELSE
    XmlError := E_XmlError.OK;
END_IF;

PointerToByteToCopy^ := 0;
CopyLen :=  Loop;
      
METHOD PUBLIC CutOff : T_MaxString
VAR_INPUT
    StartPos : UDINT;
END_VAR
VAR_OUTPUT
    CutLen : UDINT; 
    XmlError : E_XmlError;
END_VAR
VAR
    Loop : UDINT;
    PointerToByteToCut : POINTER TO BYTE;
    PointerToByteBuffer : POINTER TO BYTE;
END_VAR

Loop := 0;
PointerToByteToCut := ADR(CutOff);
PointerToByteBuffer := _PointerToStringBuffer + StartPos - 1;

WHILE PointerToByteBuffer^ <> 0 AND(Loop < SIZEOF(CutOff)) AND StartPos -1  + Loop < _Length DO
    PointerToByteToCut^ := PointerToByteBuffer^;
    Loop := Loop + 1;
    PointerToByteToCut := ADR(CutOff) + Loop;
    PointerToByteBuffer := _PointerToStringBuffer + StartPos - 1 + Loop;
END_WHILE;

IF PointerToByteBuffer^ = 0 THEN
    XmlError := E_XmlError.OK;
ELSIF Loop = SIZEOF(CutOff) THEN
    XmlError := E_XmlError.ErrorStringLen;
ELSIF StartPos - 1 + Loop = _Length THEN
    XmlError := E_XmlError.ErrorMaxBufferLen;
END_IF;

PointerToByteToCut^ := 0;
_Length := StartPos -1;

PointerToByteBuffer := _PointerToStringBuffer + StartPos - 1;
PointerToByteBuffer^ := 0;

CutLen := Loop;
      
(* 
    Find a searchstring in the buffer and returns its position.
    It's possible to add a preffered startposition within buffer
*)
METHOD PUBLIC Find : UDINT
VAR_INPUT
    SearchString : STRING;
    StartPos : UDINT;
END_VAR
VAR
    Loop : UDINT;
    Search : UDINT;
    PointerToBuffer : POINTER TO BYTE;
    PointerToSearch : POINTER TO BYTE;
END_VAR

Loop := 0;
Search := 0;

PointerToBuffer := _PointerToStringBuffer + StartPos;
PointerToSearch := ADR(SearchString);

WHILE(PointerToSearch^ <> 0 ) AND Loop + StartPos < _Length DO
    IF PointerToBuffer^ <> PointerToSearch^ THEN
        Loop := Loop + 1;
        PointerToBuffer := _PointerToStringBuffer + StartPos + Loop;
        PointerToSearch := ADR(SearchString);
        Search := 0;
    ELSE
        Search := Search + 1;
        PointerToBuffer := _PointerToStringBuffer + StartPos + Loop + Search;
        PointerToSearch := ADR(SearchString ) + Search;
    END_IF;
END_WHILE;
Find := Loop + 1 + StartPos;
      
// Gets/Sets the current length (in bytes) of the streambuffer
PROPERTY Length : UDINT


          Length := _Length;
        


          _Length := Length;
        
