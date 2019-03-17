#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Match
{
  size_t i, j;
  int dist;

  bool operator<(const Match& other) const {
    if(i < other.i)
      return true;
    if(i > other.i)
      return false;
    return dist < other.dist;
  }
};

// levenshtein distance is way too slow (at least the naive approach)
int histogramDistance(const uint8_t* s, const int lenS,
                      const uint8_t* t, const int lenT)
{
  array<uint8_t,256> histS;
  for(uint8_t& h : histS)
    h = 0;
  array<uint8_t,256> histT(histS);

  for(int i=0; i<lenS; ++i)
    ++histS[s[i]];
  for(int i=0; i<lenT; ++i)
    ++histT[t[i]];

  int dist = 0;
  for(size_t i=0; i<256; ++i) {
    dist += abs(histS[i] - histT[i]);
  }
  return dist;
}

bool readFile(const string& filename, vector<string>& text)
{
  ifstream file(filename);
  if(!file.is_open())
    return false;

  while(!file.eof()) {
    string line;
    getline(file, line);
    text.push_back(line);
  }
  return true;
}

int main(int argc, char** argv)
{
  if(argc != 4) {
    cerr << "Usage:\nfindSimilarSentences INFILE threshold OUTFILE\n";
    return 255;
  }
  const string inFilename = argv[1];
  const int distThreshold = atoi(argv[2]);
  const string outFilename = argv[3];

  vector<string> text;
  if(!readFile(inFilename, text))
    return 1;

  cout << "read " << text.size() << " lines from file\n";

  vector<Match> matches;
  matches.reserve(text.size() / 100);
  Match m;
  for(m.i=0; m.i<text.size(); ++m.i) {
    const string& s = text[m.i];
    const int slen = s.length();
    if(slen == 0)
      continue;

    for(m.j=m.i+1; m.j<text.size(); ++m.j) {
      const string& t = text[m.j];
      const int tlen = t.length();
      if(tlen == 0 || abs(slen - tlen) > distThreshold)
        continue;
      m.dist = histogramDistance(reinterpret_cast<const uint8_t*>(s.c_str()), slen,
                                 reinterpret_cast<const uint8_t*>(t.c_str()), tlen);
      if(m.dist < distThreshold)
        matches.push_back(m);
    }
    cout << m.i << " of " << text.size() << endl;
  }
  sort(matches.begin(), matches.end());
  cout << "found and sorted " << matches.size() << " matches\n";

  ofstream outFile(outFilename);
  if(!outFile.is_open())
    return 2;
  size_t lastI = text.size();
  for(const Match& m : matches) {
    if(m.i != lastI) {
      lastI = m.i;
      outFile << endl << text[m.i] << endl;
    }
    outFile << text[m.j] << endl;
  }

  return 0;
}
