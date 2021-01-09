#include<SPI.h>
#define HARDWARE_CHIP_RESET D0
#define SPI_CLOCK 1000000
void initHardwareControllerUpdater() {
  pinMode(HARDWARE_CHIP_RESET, OUTPUT);
}
struct hexLine {
  uint8_t startChar[1];
  uint8_t lineDataSize[2];
  uint8_t address[4];
  uint8_t dType[2];
  uint8_t checksum[2];
} lineEncoded;
struct hexLineD {
  uint16_t address;
  uint8_t lineDataSize;
  uint8_t dType;
  uint8_t checksum;
} lineDecoded;
bool gotStartAddress = false;
uint16_t startAddress = 0;
int programHardwareChip() {
  File codeFile = SPIFFS.open("hardwareFirmware.hex", "r");
  if (!codeFile) {
    printDebugData("Cannot open file\r\n");
    codeFile.close();
    return -1;
  }
  uint16_t codeFileSize = codeFile.size();
  printDebugData("File Loaded | Size:" + String(codeFileSize) + "\r\n");
  for (uint16_t i = 0, ii = 1; i < codeFileSize; i++, ii++) {
    //----------------------------------------------------------------------------------------
    //printDebugData("\r\nReading..");
    codeFile.read(lineEncoded.startChar, 1);//GET FIRST CHARACTER OF A LINE IN HEX FILE
    if (lineEncoded.startChar[0] == 0x3A) {   //CHECK IF FISRT CHARACTER IS ':'
      //printDebugData("Hex line " + String(ii));
      codeFile.read(lineEncoded.lineDataSize, 2);//GET SIZE OF DATA IN THAT LINE
      decodeLineHex0();//DECODE SIZE
      //-------------------------REACHED END OF FILE-----------------------------------------
      if (lineDecoded.lineDataSize != 16) {
        return 1;
      }
      //------------------------------------------------------------------------------------
      else {
        codeFile.read(lineEncoded.address, 4);//GET ADDRESS OF THAT LINE
        codeFile.read(lineEncoded.dType, 2);//GET TYPE OF DATA OF THAT LINE
        decodeLineHex1();//DECODE ADDRESS AND TYPE OF DATA
        uint8_t codeData[lineDecoded.lineDataSize * 2];
        codeFile.read(codeData, (int)lineDecoded.lineDataSize * 2);//READ DATA OF THAT LINE
        codeFile.read(lineEncoded.checksum, 2);//GET CHECKSUM OF THAT LINE
        uint8_t codeDataD[lineDecoded.lineDataSize];
        uint8_t newLine[2];
        codeFile.read(newLine, 2);//READ LINE TERMINATION \r\n
        if (newLine[0] == '\r' && newLine[1] == '\n') {
          decodeLineHex2(codeData, codeDataD);//DECODE DATA
          //printDebugData("Loaded all data");
          bool isCorr = verifyCheckSum(codeDataD);//VERIFY COMPLETE LINE DATA BY ITS CHECKSUM
          if (isCorr) {
            if (!gotStartAddress) {
              startAddress = lineDecoded.address;
              gotStartAddress = true;
            }
            for (int j = 0; j < lineDecoded.lineDataSize; j+=2) {
              spi_transaction(0x40, ((startAddress >> 8) & 0xFF), (startAddress & 0xFF), codeDataD[j]);//low byte
              delay(5);
              spi_transaction(0x48, ((startAddress >> 8) & 0xFF), (startAddress & 0xFF), codeDataD[j + 1]);//high byte
              delay(5);
              spi_transaction(0x4C, ((startAddress >> 8) & 0xFF), (startAddress & 0xFF), 0); //commit
              startAddress += 0x1;
              delay(5);
              
            }
          }
        }
      }
    }
    else {
      printDebugData("Invalid line..Code Corrupted\r\n");
      codeFile.close();
      return -1;
    }
    //------------------------------------------------------------------------------------------------
    i = i + 12 + (lineDecoded.lineDataSize * 2);
  }
  printDebugData("\r\nSome unknown error occured\r\n");
  codeFile.close();
  return -1;
}

bool verifyCheckSum(uint8_t codeD[]) {
  return true;
  /*
    uint32_t sum = 0;
    sum = lineDecoded.lineDataSize + ((lineDecoded.address >> 8) & 0xFF) + (lineDecoded.address & 0xFF);
    for (int bd = 0; bd < lineDecoded.lineDataSize; bd++) {
    sum = sum + codeD[bd];
    }
    if (((~(sum & 0xFF)) + 1) & 0xFF == lineDecoded.checksum) {
    return true;
    }
    else {
    return false;
    }
  */
}
void decodeLineHex2(uint8_t codeE[], uint8_t codeD[]) {
  lineDecoded.checksum = getByteFromHex2(lineEncoded.checksum[0], lineEncoded.checksum[1]);
  for (int j = 0, k = 0; j < (lineDecoded.lineDataSize * 2); j = j + 2, k++) {
    codeD[k] = getByteFromHex2(codeE[j], codeE[j + 1]);
  }
}
void decodeLineHex1() {
  lineDecoded.address = getByteFromHex4(lineEncoded.address[0], lineEncoded.address[1], lineEncoded.address[2], lineEncoded.address[3]);
  lineDecoded.dType = getByteFromHex2(lineEncoded.dType[0], lineEncoded.dType[1]);
}
void decodeLineHex0() {
  lineDecoded.lineDataSize = getByteFromHex2(lineEncoded.lineDataSize[0], lineEncoded.lineDataSize[1]);
}
uint8_t getByteFromHex2(uint8_t d1, uint8_t d2) {
  uint8_t res = 0;
  res = hexCharToInt(d1) << 4 | hexCharToInt(d2);
  return res;
}
uint16_t getByteFromHex4(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) {
  uint16_t res = 0;
  res = hexCharToInt(d1);
  res = res << 4;
  res = res | hexCharToInt(d2);
  res = res << 4;
  res = res | hexCharToInt(d3);
  res = res << 4;
  res = res | hexCharToInt(d4);
  return res;
}
uint8_t hexCharToInt(uint8_t c) {
  if (c >= 48 && c <= 57) {
    return (c - 48);
  }
  else if (c >= 65 && c <= 70) {
    return (10 + c - 65);
  }
  else {
    return 0;
  }
}
void activateSlaveForProgramming() {
  //RESET MODULE
  //printDebugData("Resetting\r\n");
  digitalWrite(HARDWARE_CHIP_RESET, LOW);
  delay(10);
  printDebugData("DEVICE RESET DONE\r\n");
}
void deactivateSlave() {
  //RESET MODULE
  //printDebugData("Resetting\r\n");
  digitalWrite(HARDWARE_CHIP_RESET, HIGH);
  delay(10);
  printDebugData("DEVICE RESET DONE\r\n");
}

bool updateHardwareChip() {
  bool result = false;
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
  delay(30);
  printDebugData("Preparing Chip from programming...\r\n");
  int x1 = prepareChipForProgramming();
  if (x1 == 1) {
    printDebugData(F("Programming Hardware Device...\r\n"));
    int x2 = programHardwareChip();
    if (x2 == 1) {
      int x3 = exitProgrammingMode();
      printDebugData(F("Code Upload Successful\r\n"));
      result = true;
    }
    else {
      printDebugData(F("Code Upload Unsuccessful..Error while uploading\r\n"));
    }
  }
  else {
    printDebugData(F("Code Upload Unsuccessful..Error while setting device in programming mode\r\n"));
  }
  uploadCodeInAtmega = false;
  return result;
}
int prepareChipForProgramming() {
  activateSlaveForProgramming();
  printDebugData("PUTTING DEVICE IN PROGRAMMING MODE...");
  spi_transaction(0xAC, 0x53, 0, 0);
  delay(25);
  printDebugData("ERASING CHIP...");
  spi_transaction(0xAC, 0x80, 0, 0); //erase chip for programing
  delay(50);
  printDebugData("Reading Device Signature...");
  spi_transaction(0x30, 0, 0, 0);//manufacturer
  delay(5);
  spi_transaction(0x30, 0, 1, 0);//flash size
  delay(5);
  spi_transaction(0x30, 0, 2, 0);//device group
  delay(5);
  return 1;
}
uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  SPI.transfer(a);
  SPI.transfer(b);
  SPI.transfer(c);
  uint8_t res = SPI.transfer(d);
  if(a==0x30 && c!=2)
  printDebugData(" " + String(res));
  else if(!(a==0x40 || a==0x4C || a==0x48))
  printDebugData(" " + String(res) + "\r\n");
}
int exitProgrammingMode() {
  SPI.end();
  deactivateSlave();
}
/*
bool putDeviceInProgrammingMode() {
  spi_transaction(0xAC, 0x53, 0, 0);
  return true;
}
bool setDeviceParameters() {
  Serial.write(0x42);//SET PROGRAMMING MODE PARAMTERS
  Serial.write(0x86);//DEVICE ID
  Serial.write(0);//REVISION ID 0
  Serial.write(0);//BOTH PARALLEL AND SERIAL MODE
  Serial.write(0x01);//FULL PARALLEL INTERFACE
  Serial.write(0x01);//POLLING CAN BE USED DURING SPI
  Serial.write(0x01);//PROGRAMMING INSTRUCTION SELF TIMED
  Serial.write(0x01);//1 LOCK BYTE
  Serial.write(0x03);//1 FUSE BYTE
  Serial.write(0xFF);//FLASH POLL VALUE 1
  Serial.write(0xFF);//FLASH POLL VALUE 2
  Serial.write(0xFF);//EEPROM POLL VALUE 1
  Serial.write(0xFF);//EEPROM POLL VALUE 2
  Serial.write(0);//PAGE SIZE HIGH BYTE
  Serial.write(0x80);//PAGE SIZE LOW BYTE
  Serial.write(0x04);//EEPROM SIZE HIGH BYTE
  Serial.write(0);//EEPROM SIZE LOW BYTE
  Serial.write(0);//FLASH SIZE BYTE 4(HIGH)
  Serial.write(0);//FLASH SIZE BYTE 3
  Serial.write(0x80);//FLASH SIZE BYTE 2
  Serial.write(0);//FLASH SIZE BYTE 1(LOW)
  Serial.write(0x20); //Sync_CRC_EOP
  return (checkStandardResponse(3000));
}
bool setDeviceExtendedParamters() {
  Serial.write(0x45); //SET EXTENDED FUSE BYTES
  Serial.write(0x05);
  Serial.write(0x04);
  Serial.write(0xD7);
  Serial.write(0xC2);
  Serial.write(0);
  Serial.write(0x20);//Sync_CRC_EOP
  return (checkStandardResponse(3000));
}
*/
bool checkStandardResponse(uint32_t timeout) {
  return true;
}
bool hardwareControllerUpdaterUtil() {
  bool res = false;
  int tries = 3;
  if (uploadCodeInAtmega) {
    while (tries > 0) {
      if (updateHardwareChip()) {
        res = true;
        resetVariables();
        break;
      }
      tries--;
      resetVariables();
    }
    //Serial.begin(9600);
  }
  uploadCodeInAtmega = false;
  return res;
}

void resetVariables() {
  gotStartAddress = false;
  startAddress = 0;
}
