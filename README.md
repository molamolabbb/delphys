# delphys

## How to git clone
```bash
scram p -n delphys CMSSW CMSSW_10_5_0
cd delphys/src
cmsenv
git clone git@gitlab.sscc.uos.ac.kr:analysis/delphys.git
scram b -j 20
```

## Download and untar delphes
```bash
cmsenv #in the delphys directory
cd ${CMSSW_BASE}/src/delphys/external/
wget http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.4.1.tar.gz
tar -xzvf Delphes-3.4.1.tar.gz
cd Delphes-3.4.1
sed -i s:c++0x:c++17: Makefile
make -j 4
export LD_LIBRARY_PATH=$CMSSW_BASE/src/delphys/external/Delphes-3.4.1:$LD_LIBRARY_PATH
```

## Add delphes library path into the .bashrc (or .bash_profile)
```bash
cat >> ~/.bashrc <<EOL
#Delphes Library Path
export LD_LIBRARY_PATH=$CMSSW_BASE/src/delphys/external/Delphes-3.4.1:\$LD_LIBRARY_PATH
EOL
```

## make BuildFile.xml
```bash
cd ${CMSSW_BASE}/src/delphys/analysis/test
./makeBuildFile -d ${CMSSW_BASE}/src/delphys/external/Delphes-3.4.1
```
`TODO` use Delphes in cvmfs

## For doublehiggs analyser
```bash
Please read delphys/analysis/test/hh/recipe_for_doublehiggs.sh to set external environments
```
