#include "entropy.h"
#include <stdio.h>
#include <string.h>

char test_text[] = "THEENIGMAMACHINEISACIPHERDEVICEDEVELOPEDANDUSEDINTHEEARLYTOMIDTHCENTURYTOPROTECTCOMMERCIALDIPLOMATICANDMILITARYCOMMUNICATIONITWASEMPLOYEDEXTENSIVELYBYNAZIGERMANYDURINGWORLDWARIIINALLBRANCHESOFTHEGERMANMILITARYTHEGERMANSBELIEVEDERRONEOUSLYTHATUSEOFTHEENIGMAMACHINEENABLEDTHEMTOCOMMUNICATESECURELYANDTHUSENJOYAHUGEADVANTAGEINWORLDWARIITHEENIGMAMACHINEWASCONSIDEREDTOBESOSECURETHATEVENTHEMOSTTOPSECRETMESSAGESWEREENCIPHEREDONITSELECTRICALCIRCUITSENIGMAHASANELECTROMECHANICALROTORMECHANISMTHATSCRAMBLESTHELETTERSOFTHEALPHABETINTYPICALUSEONEPERSONENTERSTEXTONTHEENIGMASKEYBOARDANDANOTHERPERSONWRITESDOWNWHICHOFLIGHTSABOVETHEKEYBOARDLIGHTSUPATEACHKEYPRESSIFPLAINTEXTISENTEREDTHELITUPLETTERSARETHEENCODEDCIPHERTEXTENTERINGCIPHERTEXTTRANSFORMSITBACKINTOREADABLEPLAINTEXTTHEROTORMECHANISMCHANGESTHEELECTRICALCONNECTIONSBETWEENTHEKEYSANDTHELIGHTSWITHEACHKEYPRESSTHESECURITYOFTHESYSTEMDEPENDSONASETOFMACHINESETTINGSTHATWEREGENERALLYCHANGEDDAILYDURINGTHEWARBASEDONSECRETKEYLISTSDISTRIBUTEDINADVANCEANDONOTHERSETTINGSTHATWERECHANGEDFOREACHMESSAGETHERECEIVINGSTATIONHASTOKNOWANDUSETHEEXACTSETTINGSEMPLOYEDBYTHETRANSMITTINGSTATIONTOSUCCESSFULLYDECRYPTAMESSAGEWHILEGERMANYINTRODUCEDASERIESOFIMPROVEMENTSTOENIGMAOVERTHEYEARSANDTHESEHAMPEREDDECRYPTIONEFFORTSTOVARYINGDEGREESTHEYDIDNOTPREVENTPOLANDFROMCRACKINGTHEMACHINEPRIORTOTHEWARENABLINGTHEALLIESTOEXPLOITENIGMAENCIPHEREDMESSAGESASAMAJORSOURCEOFINTELLIGENCEMANYCOMMENTATORSSAYTHEFLOWOFULTRACOMMUNICATIONSINTELLIGENCEFROMTHEDECRYPTIONOFENIGMALORENZANDOTHERCIPHERSSHORTENEDTHEWARSUBSTANTIALLYANDMAYEVENHAVEALTEREDITSOUTCOME";

int main(int argc,void* argv)
{
    double ips = entropy_speed_test(test_text,1000000);
    size_t text_len = strlen(test_text);
    printf("%.0lf iterations per second on %ld long string\n%.0lf cps\n",
            ips,text_len,ips*(double)text_len);
    printf("entropy: %.4f\n",calculate_entropy(test_text));
    return 0;
}