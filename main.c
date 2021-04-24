#include "entropy.h"
#include <stdio.h>
#include <string.h>
#include "enigma/enigma.h"
#include <malloc.h>

const char test_text[] = "THEENIGMAMACHINEISACIPHERDEVICEDEVELOPEDANDUSEDINTHEEARLYTOMIDTHCENTURYTOPROTECTCOMMERCIALDIPLOMATICANDMILITARYCOMMUNICATIONITWASEMPLOYEDEXTENSIVELYBYNAZIGERMANYDURINGWORLDWARIIINALLBRANCHESOFTHEGERMANMILITARYTHEGERMANSBELIEVEDERRONEOUSLYTHATUSEOFTHEENIGMAMACHINEENABLEDTHEMTOCOMMUNICATESECURELYANDTHUSENJOYAHUGEADVANTAGEINWORLDWARIITHEENIGMAMACHINEWASCONSIDEREDTOBESOSECURETHATEVENTHEMOSTTOPSECRETMESSAGESWEREENCIPHEREDONITSELECTRICALCIRCUITSENIGMAHASANELECTROMECHANICALROTORMECHANISMTHATSCRAMBLESTHELETTERSOFTHEALPHABETINTYPICALUSEONEPERSONENTERSTEXTONTHEENIGMASKEYBOARDANDANOTHERPERSONWRITESDOWNWHICHOFLIGHTSABOVETHEKEYBOARDLIGHTSUPATEACHKEYPRESSIFPLAINTEXTISENTEREDTHELITUPLETTERSARETHEENCODEDCIPHERTEXTENTERINGCIPHERTEXTTRANSFORMSITBACKINTOREADABLEPLAINTEXTTHEROTORMECHANISMCHANGESTHEELECTRICALCONNECTIONSBETWEENTHEKEYSANDTHELIGHTSWITHEACHKEYPRESSTHESECURITYOFTHESYSTEMDEPENDSONASETOFMACHINESETTINGSTHATWEREGENERALLYCHANGEDDAILYDURINGTHEWARBASEDONSECRETKEYLISTSDISTRIBUTEDINADVANCEANDONOTHERSETTINGSTHATWERECHANGEDFOREACHMESSAGETHERECEIVINGSTATIONHASTOKNOWANDUSETHEEXACTSETTINGSEMPLOYEDBYTHETRANSMITTINGSTATIONTOSUCCESSFULLYDECRYPTAMESSAGEWHILEGERMANYINTRODUCEDASERIESOFIMPROVEMENTSTOENIGMAOVERTHEYEARSANDTHESEHAMPEREDDECRYPTIONEFFORTSTOVARYINGDEGREESTHEYDIDNOTPREVENTPOLANDFROMCRACKINGTHEMACHINEPRIORTOTHEWARENABLINGTHEALLIESTOEXPLOITENIGMAENCIPHEREDMESSAGESASAMAJORSOURCEOFINTELLIGENCEMANYCOMMENTATORSSAYTHEFLOWOFULTRACOMMUNICATIONSINTELLIGENCEFROMTHEDECRYPTIONOFENIGMALORENZANDOTHERCIPHERSSHORTENEDTHEWARSUBSTANTIALLYANDMAYEVENHAVEALTEREDITSOUTCOME";


typedef struct{
    enigma_key_t key;
    float entropy;
}enigma_key_result_t;

int main(int argc,void* argv)
{
    enigma_t enigma;
    init_enigma(&enigma);
    // generate cipher text
    set_enigma_key(&enigma,"524","EGB","NF,ID,OQ,XS,ZP,YJ,MV");
    char* cipher_text = (char*)malloc(sizeof(test_text));
    if(cipher_text == NULL)
    {
        return 0;
    }
    memcpy(cipher_text,test_text,sizeof(test_text));
    enigma_encrypt_decrypt(&enigma,cipher_text);
    // get cipher codes
    int code_len = strlen(cipher_text);
    uint8_t* cipher_codes = (uint8_t*)malloc(code_len);
    if(cipher_codes == NULL)
    {
        free(cipher_text);
        return 0;
    }
    for(int i=0;i!=code_len;i++)
    {
        cipher_codes[i] = cipher_text[i] - 'A';
    }
    uint8_t* work_codes = (uint8_t*)malloc(code_len);
    if(work_codes == NULL)
    {
        free(cipher_text);
        free(cipher_codes);
        return 0;
    }
    // try keys with no plug board
    enigma_key_result_t best_key = {
        .entropy = 1000.0f,
    };
    enigma_key_t key;
    for(int i=0;i!=26;i++)
    {
        key.plug_board.key_map[i] = i;
    }
    for(int rotor1=0;rotor1!=5;rotor1++)
    {
        key.rotor_num[0] = rotor1;
        for(int rotor2=0;rotor2!=5;rotor2++)
        {
            if(rotor1 == rotor2)
            {
                continue;
            }
            key.rotor_num[1] = rotor2;
            for(int rotor3=0;rotor3!=5;rotor3++)
            {
                if(rotor1 == rotor3 || rotor2 == rotor3)
                {
                    continue;
                }
                key.rotor_num[2] = rotor3;
                for(int offset1 = 0;offset1 != 26;offset1++)
                {
                    key.rotor_offset[0] = offset1;
                    for(int offset2 = 0;offset2 != 26;offset2++)
                    {
                        key.rotor_offset[1] = offset2;
                        for(int offset3 = 0;offset3 != 26;offset3++)
                        {
                            key.rotor_offset[2] = offset3;
                            set_enigma_key_from_save(&enigma,&key);
                            memcpy(work_codes,cipher_codes,code_len);
                            enigma_encrypt_decrypt_direct(&enigma,work_codes,code_len);
                            float entropy = calculate_entropy_direct(work_codes,code_len);
                            if(entropy < best_key.entropy)
                            {
                                memcpy(&best_key.key,&key,sizeof(key));
                                best_key.entropy = entropy;
                            }
                        }
                    }
                }
            }
        }
    }
    printf("Best result\nrotors: %d %d %d\noffset: %d %d %d\nentropy: %.4f\n",
            best_key.key.rotor_num[0],best_key.key.rotor_num[1],best_key.key.rotor_num[2],
            best_key.key.rotor_offset[0],best_key.key.rotor_offset[1],best_key.key.rotor_offset[2],
            best_key.entropy);

    free(cipher_text);
    free(cipher_codes);
    free(work_codes);
    return 0;
}