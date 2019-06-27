///////////////////////////////////////////////////////////////////////////////
//  SORT: A Simple, Online and Realtime Tracker
//
//  This is a C++ reimplementation of the open source tracker in
//  https://github.com/abewley/sort
//  Based on the work of Alex Bewley, alex@dynamicdetection.com, 2016
//
//  Cong Ma, mcximing@sina.cn, 2016
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
#include <unistd.h> // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include <set>

#include "OnlineSortTracker.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

// global variables for counting
#define CNUM 20
int total_frames = 0;
double total_time = 0.0;

void TestSORT(string seqName);

int main()
{
    vector<string> sequences = { "PETS09-S2L1", "TUD-Campus", "TUD-Stadtmitte", "ETH-Bahnhof", "ETH-Sunnyday", "ETH-Pedcross2", "KITTI-13", "KITTI-17", "ADL-Rundle-6", "ADL-Rundle-8", "Venice-2" };
    for (auto seq : sequences)
        TestSORT(seq);

    return 0;
}

void getData(string seqName, vector<vector<TrackingBox>>& data) {
    cout << "Processing " << seqName << "..." << endl;

    // 1. read detection file
    ifstream detectionFile;
    string detFileName = "data/" + seqName + "/det.txt";
    detectionFile.open(detFileName);

    if (!detectionFile.is_open())
    {
        cerr << "Error: can not find file " << detFileName << endl;
        return;
    }

    string detLine;
    istringstream ss;
    vector<TrackingBox> detData;
    char ch;
    float tpx, tpy, tpw, tph;

    while ( getline(detectionFile, detLine) )
    {
        TrackingBox tb;

        ss.str(detLine);
        ss >> tb.frame >> ch >> tb.id >> ch;
        ss >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;
        ss.str("");

        // std::cout << tb.frame << " " << tb.id << " " << tpx << " " << tpy << " " << tpw << " " << tph << std::endl;

        tb.box = Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph));
        detData.push_back(tb);
    }
    detectionFile.close();

    // 2. group detData by frame
    size_t maxFrame = 0;
    for (auto tb : detData) // find max frame number
    {
        if (maxFrame < tb.frame)
            maxFrame = tb.frame;
    }

    vector<TrackingBox> tempVec;
    for (int fi = 0; fi < maxFrame; fi++)
    {
        for (auto tb : detData)
            if (tb.frame == fi + 1) // frame num starts from 1
                tempVec.push_back(tb);
        data.push_back(tempVec);
        tempVec.clear();
    }
}

void TestSORT(string seqName)
{
    vector<vector<TrackingBox>> data;
    getData(seqName, data);

    // prepare result file.
    ofstream resultsFile;
    string resFileName = "output/" + seqName + ".txt";
    resultsFile.open(resFileName);

    if (!resultsFile.is_open())
    {
        cerr << "Error: can not create file " << resFileName << endl;
        return;
    }

    OnlineSortTracker ost;

    //////////////////////////////////////////////
    // main loop
    for (size_t fi = 0; fi < data.size(); fi++)
    {
        total_frames++;
        ost.process(data[fi]);
    }

    string result;
    ost.print(result);
    resultsFile << result;

    resultsFile.close();

    std::cout << result << std::endl;
}
