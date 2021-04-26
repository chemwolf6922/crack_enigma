#include "entropy.h"
#include <stdio.h>
#include <string.h>
#include "enigma/enigma.h"
#include <malloc.h>
#include <sys/time.h>
#include "heap.h"

const char test_text[] = "THEENIGMAMACHINEISACIPHERDEVICEDEVELOPEDANDUSEDINTHEEARLYTOMIDTHCENTURYTOPROTECTCOMMERCIALDIPLOMATICANDMILITARYCOMMUNICATIONITWASEMPLOYEDEXTENSIVELYBYNAZIGERMANYDURINGWORLDWARIIINALLBRANCHESOFTHEGERMANMILITARYTHEGERMANSBELIEVEDERRONEOUSLYTHATUSEOFTHEENIGMAMACHINEENABLEDTHEMTOCOMMUNICATESECURELYANDTHUSENJOYAHUGEADVANTAGEINWORLDWARIITHEENIGMAMACHINEWASCONSIDEREDTOBESOSECURETHATEVENTHEMOSTTOPSECRETMESSAGESWEREENCIPHEREDONITSELECTRICALCIRCUITSENIGMAHASANELECTROMECHANICALROTORMECHANISMTHATSCRAMBLESTHELETTERSOFTHEALPHABETINTYPICALUSEONEPERSONENTERSTEXTONTHEENIGMASKEYBOARDANDANOTHERPERSONWRITESDOWNWHICHOFLIGHTSABOVETHEKEYBOARDLIGHTSUPATEACHKEYPRESSIFPLAINTEXTISENTEREDTHELITUPLETTERSARETHEENCODEDCIPHERTEXTENTERINGCIPHERTEXTTRANSFORMSITBACKINTOREADABLEPLAINTEXTTHEROTORMECHANISMCHANGESTHEELECTRICALCONNECTIONSBETWEENTHEKEYSANDTHELIGHTSWITHEACHKEYPRESSTHESECURITYOFTHESYSTEMDEPENDSONASETOFMACHINESETTINGSTHATWEREGENERALLYCHANGEDDAILYDURINGTHEWARBASEDONSECRETKEYLISTSDISTRIBUTEDINADVANCEANDONOTHERSETTINGSTHATWERECHANGEDFOREACHMESSAGETHERECEIVINGSTATIONHASTOKNOWANDUSETHEEXACTSETTINGSEMPLOYEDBYTHETRANSMITTINGSTATIONTOSUCCESSFULLYDECRYPTAMESSAGEWHILEGERMANYINTRODUCEDASERIESOFIMPROVEMENTSTOENIGMAOVERTHEYEARSANDTHESEHAMPEREDDECRYPTIONEFFORTSTOVARYINGDEGREESTHEYDIDNOTPREVENTPOLANDFROMCRACKINGTHEMACHINEPRIORTOTHEWARENABLINGTHEALLIESTOEXPLOITENIGMAENCIPHEREDMESSAGESASAMAJORSOURCEOFINTELLIGENCEMANYCOMMENTATORSSAYTHEFLOWOFULTRACOMMUNICATIONSINTELLIGENCEFROMTHEDECRYPTIONOFENIGMALORENZANDOTHERCIPHERSSHORTENEDTHEWARSUBSTANTIALLYANDMAYEVENHAVEALTEREDITSOUTCOME";


typedef struct{
    enigma_key_t key;
    float entropy;
}enigma_key_result_t;

enigma_key_result_t* crack_rotors(const char* cipher_text,int key_num);
int crack_plug_board(const char* cipher_text,enigma_key_result_t* key_result);


void dump_num(void *src, int len)
{
    for (int i = 0; i != len; i++)
    {
        printf("%02d ", *(uint8_t *)(src + i));
    }
    printf("\n");
}

void dump_int(int *src, int len)
{
    for (int i = 0; i != len; i++)
    {
        printf("%02d ", *(src + i));
    }
    printf("\n");
}

void print_key_result(enigma_key_result_t* result)
{
    enigma_key_t* key = &result->key;
    printf("Rotors:  %d %d %d\n",key->rotor_num[0]+1,key->rotor_num[1]+1,key->rotor_num[2]+1);
    printf("Offsets: %c %c %c\n",key->rotor_offset[0]+'A',key->rotor_offset[1]+'A',key->rotor_offset[2]+'A');
    printf("Plug board:\n");
    for(int i=0;i!=26;i++)
    {
        printf("%c,",key->plug_board.key_map[i]+'A');
    }
    printf("\n");
    printf("Entropy: %.4f\n",result->entropy);
}


int main(int argc,void* argv)
{
    
    enigma_t enigma;
    init_enigma(&enigma);
    // generate cipher text
    set_enigma_key(&enigma,"234","MIB","IR,YM,QK,JP,CH,GX,SD,NU,AZ,BV");
    char* cipher_text = (char*)malloc(sizeof(test_text));
    if(cipher_text == NULL)
    {
        return 0;
    }
    memcpy(cipher_text,test_text,sizeof(test_text));
    enigma_encrypt_decrypt(&enigma,cipher_text);

    struct timeval start,stop;
    gettimeofday(&start,NULL);
    // crack rotors
    int num_of_keys = 100;
    enigma_key_result_t* key_crack = crack_rotors(cipher_text,num_of_keys);
    if(key_crack == NULL)
    {
        free(cipher_text);
        return 0;
    }
    // crack plug board
    for(int i=0;i!=num_of_keys;i++)
    {
        crack_plug_board(cipher_text,&key_crack[i]);
    }
    // select best result
    enigma_key_result_t* crack_result = &key_crack[0];
    for(int i=1;i!=num_of_keys;i++)
    {
        if(key_crack[i].entropy < crack_result->entropy)
        {
            crack_result = &key_crack[i];
        }
    }
    gettimeofday(&stop,NULL);
    print_key_result(crack_result);
    printf("Time used to crack enigma: %.6fs\n",
            ((float)(stop.tv_sec*1000000+stop.tv_usec-start.tv_sec*1000000-start.tv_usec))*0.000001);

    free(key_crack);
    free(cipher_text);
    return 0;
}

bool compare_enigma_key_result(void* A,void* B)
{
    return ((enigma_key_result_t*)A)->entropy > ((enigma_key_result_t*)B)->entropy;
}

enigma_key_result_t* crack_rotors(const char* cipher_text,int key_num)
{
    enigma_t enigma;
    init_enigma(&enigma);
    // crate heap
    heap_handle_t heap = NULL;
    heap_create(&heap,key_num,sizeof(enigma_key_result_t),compare_enigma_key_result);
    // get cipher codes
    int code_len = strlen(cipher_text);
    uint8_t* cipher_codes = (uint8_t*)malloc(code_len);
    if(cipher_codes == NULL)
    {
        return NULL;
    }
    for(int i=0;i!=code_len;i++)
    {
        cipher_codes[i] = cipher_text[i] - 'A';
    }
    uint8_t* work_codes = (uint8_t*)malloc(code_len);
    if(work_codes == NULL)
    {
        free(cipher_codes);
        return NULL;
    }
    // try keys with no plug board
    // enigma_key_result_t best_key = {
    //     .entropy = 1000.0f,
    // };
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
                            enigma_key_result_t result = {
                                .entropy = entropy,
                            };
                            memcpy(&result.key,&key,sizeof(enigma_key_t));
                            heap_add(heap,&result);
                            // if(entropy < best_key.entropy)
                            // {
                            //     memcpy(&best_key.key,&key,sizeof(key));
                            //     best_key.entropy = entropy;
                            //     // TODO: save TOP K to avoid missing the right key
                            // }
                        }
                    }
                }
            }
        }
    }
    free(cipher_codes);
    free(work_codes);

    // printf("Best rotor result\nrotors: %d %d %d\noffset: %c %c %c\nEntropy: %.4f\n",
    //         best_key.key.rotor_num[0]+1,best_key.key.rotor_num[1]+1,best_key.key.rotor_num[2]+1,
    //         best_key.key.rotor_offset[0]+'A',best_key.key.rotor_offset[1]+'A',best_key.key.rotor_offset[2]+'A',
    //         best_key.entropy);    
    int temp;
    enigma_key_result_t* key_result = heap_get_all(heap,&temp);
    heap_delete(heap);
    return key_result;
}

/* This will write the plug board into the given key */
int crack_plug_board(const char* cipher_text,enigma_key_result_t* key_result)
{
    enigma_t enigma;
    init_enigma(&enigma);
    // prepare buffers
    int code_len = strlen(cipher_text);
    uint8_t* cipher_codes = (uint8_t*)malloc(code_len);
    if(cipher_text==NULL)
    {
        return -1;
    }
    for(int i=0;i!=code_len;i++)
    {
        cipher_codes[i] = cipher_text[i] - 'A';
    }
    uint8_t* work_codes = (uint8_t*)malloc(code_len);
    if(work_codes == NULL)
    {
        free(cipher_codes);
        return -1;
    }
    // set best key
    enigma_key_t* key = &key_result->key;
    enigma_key_result_t best_key;
    memcpy(best_key.key.rotor_num,key->rotor_num,sizeof(best_key.key.rotor_num));
    memcpy(best_key.key.rotor_offset,key->rotor_offset,sizeof(best_key.key.rotor_offset));
    // try wires
    wire_t wires[13];
    memset(wires,0,sizeof(wires));
    // start form no wire
    set_enigma_key_direct(&enigma,key->rotor_num,key->rotor_offset,NULL,0);
    memcpy(work_codes,cipher_codes,code_len);
    enigma_encrypt_decrypt_direct(&enigma,work_codes,code_len);
    best_key.entropy = calculate_entropy_direct(work_codes,code_len);
    // n: number of wires
    for(int n=1;n<=13;n++)
    {
        wire_t wire_found;
        wire_t* wire = &wires[n-1];
        bool new_wire_found = false;

        for(int A=0;A!=26;A++)
        {
            for(int j=0;j<(n-1);j++)
            {
                if(A==wires[j].key_A || A==wires[j].key_B)
                {
                    continue;
                }
            }
            wire->key_A = A;
            for(int B=0;B!=26;B++)
            {
                for(int j=0;j<(n-1);j++)
                {
                    if(B==wires[j].key_A || B==wires[j].key_B)
                    {
                        continue;
                    }
                }
                wire->key_B = B;
                set_enigma_key_direct(&enigma,key->rotor_num,key->rotor_offset,wires,n);
                memcpy(work_codes,cipher_codes,code_len);
                enigma_encrypt_decrypt_direct(&enigma,work_codes,code_len);
                float entropy = calculate_entropy_direct(work_codes,code_len);
                if(entropy<best_key.entropy)
                {
                    memcpy(best_key.key.plug_board.key_map,enigma.plug_board.key_map,26);
                    best_key.entropy = entropy;
                    wire_found.key_A = A;
                    wire_found.key_B = B;
                    new_wire_found = true;
                }
            }
        }
        if(!new_wire_found)
        {
            // printf("Found %d wires\n",n-1);
            break;
        }
        else
        {
            wire->key_A = wire_found.key_A;
            wire->key_B = wire_found.key_B;
            // printf("Found wire: %c-%c ,Entropy: %.4f\n",
                    // wire_found.key_A+'A',wire_found.key_B+'A',best_key.entropy);
        }
    }
    free(work_codes);
    free(cipher_codes);
    // copy over results
    memcpy(key->plug_board.key_map,best_key.key.plug_board.key_map,26);
    key_result->entropy = best_key.entropy;
    return 0;
}