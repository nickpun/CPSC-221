
#include "stats.h"
#include <math.h>

stats::stats(PNG & im){
/* your code here */
    for(int i=0; i<im.width(); i++) {
        for(int j=0; j<im.height(); j++) {
            double temphx = 0.;
            double temphy = 0.;
            double temps  = 0.;
            double templ  = 0.;
            vector<int> temphist(36,0);
            for(int jt=0; jt<=j; jt++) {
                temphy += sin(pi/180*im.getPixel(i,jt)->h);
            }
            for(int it=0; it<=i; it++) {
                temphx += cos(pi/180*im.getPixel(it,j)->h);
                for(int jt=0; jt<=j; jt++) {
                    HSLAPixel* temppx = im.getPixel(it,ij);
                    temps += temppx->s;
                    templ += temppx->l;
                    temphist[floor((int) (temppx->h)/10)]++;
                }
            }
            sumHueX[i][j] = temphx;
            sumHueY[i][j] = temphy;
            sumSat [i][j] = temps;
            sumLum [i][j] = templ;
            hist   [i][j] = temphist;
        }
    }
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
/* your code here */
    return (long) (lr.first-ul.first+1)*(lr.second-ul.second+1);
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
/* your code here */
    double havg = 0.;
    double savg;
    double lavg;
    if(ul.second == 0) {
        for(int it=ul.first; it<=lr.first; it++) {
            havg += 180/pi*arcsin(sumHueY[it][lr.second]);
        }
    } else {
        for(int it=ul.first; it<=lr.first; it++) {
            havg += 180/pi*arcsin(sumHueY[it][lr.second]) - 180/pi*arcsin(sumHueY[it][ul.second-1]);
        }
    }
    double sul = sumSat[ul.first-1][ul.second-1];
    double sur = sumSat[lr.first  ][ul.second-1];
    double sll = sumSat[ul.first-1][lr.second  ];
    double slr = sumSat[lr.first  ][lr.second  ];
    double lul = sumLum[ul.first-1][ul.second-1];
    double lur = sumLum[lr.first  ][ul.second-1];
    double lll = sumLum[ul.first-1][lr.second  ];
    double llr = sumLum[lr.first  ][lr.second  ];
    if(ul.first != 0 && ul.second != 0) {
        savg = slr-sll-sur+sul;
        lavg = llr-lll-lur+lul;
    } else if(ul.first != 0 && ul.second == 0) {
        savg = slr-sll;
        lavg = llr-lll;
    } else if(ul.first == 0 && ul.second != 0) {
        savg = slr-sur;
        lavg = llr-lur;
    } else {
        savg = slr;
        lavg = llr;
    }
    havg = havg/rectArea(ul,lr);
    savg = savg/rectArea(ul,lr);
    lavg = lavg/rectArea(ul,lr);
    return new HSLAPixel(havg,savg,lavg);
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){
/* your code here */
    vector<int> temphist;
    vector<int> histul = hist[ul.first-1][ul.second-1];
    vector<int> histur = hist[lr.first  ][ul.second-1];
    vector<int> histll = hist[ul.first-1][lr.second  ];
    vector<int> histlr = hist[lr.first  ][lr.second  ];
    temphist = histlr;
    if(ul.first != 0 && ul.second != 0) {
        vecSub(&temphist,&histll);
        vecSub(&temphist,&histur);
        vebAdd(&temphist,&histul);
    } else if(ul.first != 0 && ul.second == 0) {
        vecSub(&temphist,&histll);
    } else if(ul.first == 0 && ul.second != 0) {
        vecSub(&temphist,&histur);
    }
    return temphist;
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

    double entropy = 0.;

/* your code here */

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){
/* your code here */
    vector<int> temphist = buildHist(ul,lr);
    double en = 0.;
    for(int kt=0; kt<36; kt++) {
        if(temphist[kt] > 0) {
            double p = (double) temphist[kt]/(double) rectArea(ul,lr);
            en -= p*log2(p);
        }
    }
    return en;
}

void stats::vecAdd(vector<int> & a, vector<int> & b) {
    std::transform(*a.begin(),*a.end(),*b.begin(),*a.begin(),std::plus<int>());
}

void stats::vecSub(vector<int> & a, vector<int> & b) {
    std::transform(*a.begin(),*a.end(),*b.begin(),*a.begin(),std::minus<int>());
}