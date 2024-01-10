#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
FILE *resultFile, *backingStore, *address;

char addresses[10];
int sizeOfFrame, sizeOfPage, sizeOfTLB = 16;
int nooper, noframes, nofaults, noTLB_Hits;
int index_TLB = 0, index_PT = -1, frame = 0;
int phy_addr; int tmpIndTBL = 0;

typedef struct {
    int page_num;
    int frame_num;
} page;

void chooseOutput(char *frames);
double getPageFaultRate (int nofaults, int nooper);
double getTLBHitRate (int noTLB_Hits, int nooper);

int main(int argc, char *argv[]) {
    chooseOutput(argv[1]); // read argv[1], 128 or 256, so it can initialize accordingly to create
    backingStore = fopen(argv[2], "r");
    address = fopen(argv[3], "r");

    signed char phy_Mem[sizeOfFrame][256];
    for (int i = 0; i < sizeOfFrame; i++) {
        for (int j = 0; j < 256; j++) {
            phy_Mem[i][j] = 0;
        }
    }

    page pt[sizeOfPage];
    for (int i = 0; i < sizeOfPage; i++) {
        pt[i].page_num = -1;
        pt[i].frame_num = -1;
    }
    
    page TLB[sizeOfTLB];
    for (int i = 0; i < sizeOfTLB; i++) {
        TLB[i].page_num = -1;
        TLB[i].frame_num = -1;
    }   

    while (fgets(addresses, sizeof(addresses), address)) {
        nooper += 1;
        index_PT = -1;
        int log_addr = atoi(addresses);
        int log_p_num = (log_addr >> 8) & 0x00FF;
        int log_p_offset = log_addr & 0x00FF;
        int tblMatch = 0; // 1 - TLB, 0 - Page Table

        for (int i = 0; i < sizeOfTLB; i++) { // try to look inside of TLB first before proceeding in Page Table 
            if (TLB[i].page_num == log_p_num) {
                frame = TLB[i].frame_num;
                noTLB_Hits++;
                tblMatch = 1;

                if (sizeOfPage == 128) {
                    for (int j = 0; j < sizeOfPage; j++) {
                        if (pt[j].page_num == log_p_num) {
                            index_TLB = j;
                            break;
                        }
                    }
                    page entry_TLB = pt[index_TLB];
                    frame = (noframes + 1) % 128;
                    for (int k = index_TLB; k > 0; k--) {
                        pt[k] = pt[k-1];
                    }
                    pt[0] = entry_TLB;
                    frame = pt[0].frame_num;
                }
                break;
            }
        }

        if (tblMatch == 0) { // not found in TLB, proceed in Page Table
            if (sizeOfPage == 256) { 
                if (pt[log_p_num].frame_num >= 0) {
                    frame = pt[log_p_num].frame_num;
                    index_PT = log_p_num;
                }
            } else if (sizeOfPage == 128) {
                for (int i = 0; i < sizeOfPage; i++) {
                    if (pt[i].page_num == log_p_num && pt[i].frame_num >= 0) {
                        index_PT = i;
                        break;
                    }
                }
            
                if (index_PT != -1) {
                    page entry_PT = pt[index_PT];
                    for (int i = index_PT; i > 0; i--) {
                        page entry2_PT = pt[i-1];
                        pt[i] = entry2_PT;
                    } 
                    pt[0] = entry_PT;
                    frame = pt[0].frame_num;
                }
            }

            if (index_PT == -1) { // update page table when it gets the fault
                nofaults++;
                if (sizeOfFrame == 256) {
                    frame = noframes % sizeOfFrame;
                    pt[log_p_num].frame_num = frame;
                    noframes++;
                } else if (sizeOfFrame == 128) {
                    int fframe = pt[127].frame_num;
                    for (int i = 127; i > 0; i--) {
                        page tmp = pt[i-1];
                        pt[i] = tmp;
                    }
                    pt[0].page_num = log_p_num;
                    if (noframes > 127) {
                        pt[0].frame_num = fframe;
                        frame = pt[0].frame_num;
                    } else {
                        frame = noframes % 128;
                        pt[0].frame_num = frame;
                    }
                    noframes++;
                }
                // error occurs fetching one gives 0 by reading it OR not matching with the calculated one
                if ((fseek(backingStore, log_p_num * 256, SEEK_SET) != 0) || (fread(phy_Mem[frame], sizeof(signed char), 256, backingStore) == 0)) {
                    return -1;
                }
            }
            TLB[tmpIndTBL].page_num = log_p_num;
            TLB[tmpIndTBL].frame_num = frame;
            tmpIndTBL++;
            tmpIndTBL %= sizeOfTLB;
        }
        phy_addr = (frame << 8) | log_p_offset;
        fprintf(resultFile, "%d,%d,%d\n", log_addr, phy_addr, phy_Mem[frame][log_p_offset]);
    }
    fprintf(resultFile, "Page Faults Rate, %.2f%%,\n", getPageFaultRate(nofaults, nooper));
    fprintf(resultFile, "TLB Hits Rate, %.2f%%,", getTLBHitRate(noTLB_Hits, nooper));
    fclose(resultFile); fclose(backingStore); fclose(address);
}

void chooseOutput(char *frames) {
    if (strcmp(frames, "128") == 0) {
        resultFile = fopen("output128.csv", "w+");
        sizeOfFrame = 128;
        sizeOfPage = 128;
    } else if (strcmp(frames, "256") == 0) {
        resultFile = fopen("output256.csv", "w+");
        sizeOfFrame = 256;
        sizeOfPage = 256;
    }
}

double getPageFaultRate (int nofaults, int nooper) {
    return ((double) nofaults / (double) nooper) * 100;
}

double getTLBHitRate (int noTLB_Hits, int nooper) {
    return ((double) noTLB_Hits / (double) nooper) * 100;
}