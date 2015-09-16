/*****************************************************************
Name:			datatypes.h

Description:
	Data types used by the Optotrak sample programs.

*****************************************************************/

#define OPTO_APP_ERROR_CODE					3000
#define OPTOTRAK_REVISION_UNKNOWN_FLAG		0

typedef struct OptotrakSettingsStruct
{
	int nMarkers;
	int nThreshold;
	int nMinimumGain;
	int nStreamData;
	float fFrameFrequency;
	float fMarkerFrequency;
	float fDutyCycle;
	float fVoltage;
	float fCollectionTime;
	float fPreTriggerTime;
	unsigned int uFlags;
} OptotrakSettings;


typedef struct OptotrakStatusStruct
{
	int nSensors;
	int nOdaus;
	int nRigidBodies;
	OptotrakSettings dtSettings;
} OptotrakStatus;


typedef struct ApplicationDeviceInformationStruct
{
	char
		szName[DEVICE_MAX_PROPERTY_STRING],
		szFiringSequence[DEVICE_MAX_MARKERS],
		szSromFiringSequence[DEVICE_MAX_MARKERS],
		szSromPartNumber[DEVICE_MAX_PROPERTY_STRING],
		szSromManufacturer[DEVICE_MAX_PROPERTY_STRING],
		szSerialNumber[DEVICE_MAX_PROPERTY_STRING],
		szFeatureKey[DEVICE_MAX_PROPERTY_STRING],
		szMkrPortEnc[DEVICE_MAX_PROPERTY_STRING];
	int
		nMarkersAvailable,
		nMarkersToActivate,
		n3020MarkersToActivate,
		nStartMarkerPeriod,
		n3020StartMarkerPeriod,
		nPort,
		nOrder,
		nSubPort,
		nVLEDs,
		nSwitches,
		nToolPorts,
		nStatus,
		nSromRigidMarkers,
		nSromRigidPositions,
		nSromNormalMarkers,
		nSromNormals,
		nSromToolType,
		nScanMode,
		nBumpSensor,
		nWireless,
		nBeeper,
		nDeviceType,
		nSmartMarkers;
	long
		lSromSerialNumber,
		lStateFlags,
		lTipId;
	boolean
		b3020Capability,
		bHasROM;
} ApplicationDeviceInformation;
