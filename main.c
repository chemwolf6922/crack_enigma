#include "entropy.h"
#include <stdio.h>
#include <string.h>

char test_text[] = "THEENIGMAMACHINEISACIPHERDEVICEDEVELOPEDANDUSEDINTHEEARLYTOMIDTHCENTURYTOPROTECTCOMMERCIALDIPLOMATICANDMILITARYCOMMUNICATIONITWASEMPLOYEDEXTENSIVELYBYNAZIGERMANYDURINGWORLDWARIIINALLBRANCHESOFTHEGERMANMILITARYTHEGERMANSBELIEVEDERRONEOUSLYTHATUSEOFTHEENIGMAMACHINEENABLEDTHEMTOCOMMUNICATESECURELYANDTHUSENJOYAHUGEADVANTAGEINWORLDWARIITHEENIGMAMACHINEWASCONSIDEREDTOBESOSECURETHATEVENTHEMOSTTOPSECRETMESSAGESWEREENCIPHEREDONITSELECTRICALCIRCUITSENIGMAHASANELECTROMECHANICALROTORMECHANISMTHATSCRAMBLESTHELETTERSOFTHEALPHABETINTYPICALUSEONEPERSONENTERSTEXTONTHEENIGMASKEYBOARDANDANOTHERPERSONWRITESDOWNWHICHOFLIGHTSABOVETHEKEYBOARDLIGHTSUPATEACHKEYPRESSIFPLAINTEXTISENTEREDTHELITUPLETTERSARETHEENCODEDCIPHERTEXTENTERINGCIPHERTEXTTRANSFORMSITBACKINTOREADABLEPLAINTEXTTHEROTORMECHANISMCHANGESTHEELECTRICALCONNECTIONSBETWEENTHEKEYSANDTHELIGHTSWITHEACHKEYPRESSTHESECURITYOFTHESYSTEMDEPENDSONASETOFMACHINESETTINGSTHATWEREGENERALLYCHANGEDDAILYDURINGTHEWARBASEDONSECRETKEYLISTSDISTRIBUTEDINADVANCEANDONOTHERSETTINGSTHATWERECHANGEDFOREACHMESSAGETHERECEIVINGSTATIONHASTOKNOWANDUSETHEEXACTSETTINGSEMPLOYEDBYTHETRANSMITTINGSTATIONTOSUCCESSFULLYDECRYPTAMESSAGEWHILEGERMANYINTRODUCEDASERIESOFIMPROVEMENTSTOENIGMAOVERTHEYEARSANDTHESEHAMPEREDDECRYPTIONEFFORTSTOVARYINGDEGREESTHEYDIDNOTPREVENTPOLANDFROMCRACKINGTHEMACHINEPRIORTOTHEWARENABLINGTHEALLIESTOEXPLOITENIGMAENCIPHEREDMESSAGESASAMAJORSOURCEOFINTELLIGENCEMANYCOMMENTATORSSAYTHEFLOWOFULTRACOMMUNICATIONSINTELLIGENCEFROMTHEDECRYPTIONOFENIGMALORENZANDOTHERCIPHERSSHORTENEDTHEWARSUBSTANTIALLYANDMAYEVENHAVEALTEREDITSOUTCOME";

int main(int argc,void* argv)
{
    printf("%.0lf iterations per second on %ld long string\n",
            entropy_speed_test(test_text,1000000),
            strlen(test_text));
    printf("entropy: %f\n",calculate_entropy(test_text));
    return 0;
}