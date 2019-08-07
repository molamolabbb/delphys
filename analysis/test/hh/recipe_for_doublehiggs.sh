#### Set CMSSW Environment and Get Github Delphys Repository
#scram p -n delPhys CMSSW CMSSW_9_4_4
#cd delPhys/src
#cmsenv
cd $CMSSW_BASE/src
git clone git@github.com:"your_user_name"/delphys.git #"your_user_name" should be replaced by real user name
#build analysers 
scram b -j 20
getFiles
#### Delphes  
cd $CMSSW_BASE/src/delphys/external/
wget http://cp3.irmp.ucl.ac.be/downloads/Delphes-3.4.1.tar.gz
tar -xzvf Delphes-3.4.1.tar.gz
rm Delphes-3.4.1.tar.gz
cd Delphes-3.4.1
sed -i s:c++0x:c++17: Makefile #set c++ version as 17th in Makefile
make -j 4
export LD_LIBRARY_PATH=$CMSSW_BASE/src/delphys/external/Delphes-3.4.1:$LD_LIBRARY_PATH
#make BuildFile.xml with the script
cd $CMSSW_BASE/src/delphys/analysis/test/
./makeBuildFile -d $CMSSW_BASE/src/delphys/external/Delphes-3.4.1/
#add delphes library path into the .bashrc (or .bash_profile)
cat >> ~/.bashrc <<EOL
# Delphes Library Path
export LD_LIBRARY_PATH=$CMSSW_BASE/src/delphys/external/Delphes-3.4.1:\$LD_LIBRARY_PATH
EOL
#####################################################################
#set right path in the nano/analysis/bin/BuildFile.xml
#Lines below should be added in the analysis/bin/BuildFile.xml
cd $CMSSW_BASE/src/delphys/analysis/bin/
sed -i 's,\(\doubleHiggsAnalyser.*\),\1\n  \<flags CXXFLAGS=\"-lMinuit\"/\>,g' $CMSSW_BASE/src/delphys/analysis/bin/BuildFile.xml

scram b -j12
