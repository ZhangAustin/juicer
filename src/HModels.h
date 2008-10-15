/*
 * HModels.cpp:
 *
 * This is the adapter class that connects HTK to Juicer via the API
 *
 * Much of the code is copied from HTKTools/HHEd.c or HTKLVrec/HDecode.c
 *
 * Environment variables for this module are:
 *
 *  Created on: 08-Aug-2008
 *      Author: Vincent Wan, University of Sheffield, UK.
 *
 *
 *
 *
 *  INSTRUCTIONS for initialising this class:
 *
 *  	HModels m = new HModels();
 *  	m::SetHTKCFG("HTKConfigFileName");
 *  	m::SetHTKModelsList("modelListFilename");
 *  	m::Load("MMF.ascii");
 *
 *  That's it. If additional HTK options (e.g. ones that are specified on the
 *  HTK command line) are required then they can be included in the function
 *  HModels.cpp::InitialiseHTK() and HModels.cpp::ParseArgs().
 *
 *
 *
 *  KNOWN BUGS:
 *  	Memory not deleted properly. See destructor in HModels.cpp
 *
 */



#ifndef HMODELS_INC
#define HMODELS_INC

#include "Models.h"

namespace Juicer
{

	/**
	 * Includes from HHEd.c
	 */
	#include "HShell.h"
	#include "HMem.h"
	#include "HMath.h"
	#include "HAudio.h"
	#include "HWave.h"
	#include "HParm.h"
	#include "HLabel.h"
	#include "HModel.h"

	/**
	 * Juicer's transition matrix structure.
	 */
	struct HModelsTransMatrix
	{
		int   nStates ;
		int   *nSucs ;
		int   **sucs ;
		real  **logProbs ;
	};

	/**
	 * An element of the output probability cache
	 */
	struct HModelCacheElement
	{
		bool isEmpty;
		real logProb;
	};

    /**
     * HTK models using HTKLib
     */
    class HModels : public Models
    {
    public:
        HModels() ;
        virtual ~HModels();

        /*
         * Set the name of the file that stores the HTK configuration
         */
        void SetHTKCFG( const char * configFName );
		/*
		 * Function to set the name HTK model list file
    	 * e.g. /share/spandh.ami1/ami/amiasr/decoder/test/data/models-rt06/xwrd.clustered.mlist"
		 */
        void SetHTKModelsList( const char * htkModelsList );

        /*
         * Required functions as specified by the Models.h interface
         */
        void Load( const char *phonesListFName , const char *priorsFName ,
                int statesPerModel ) ;
        void Load( const char *htkModelsFName ,
                bool removeInitialToFinalTransitions_=false ) ;

        void readBinary( const char *fName ) ;
        void output( const char *fName , bool outputBinary ) ;
        void outputStats( FILE *fd=stdout ) ;
        void newFrame( int frame , const real *input ) ;
        real calcOutput( int hmmInd , int stateInd ) ;
        real calcOutput( int gmmInd ) ;

        int getNumHMMs() ;
        int getCurrFrame() ;
        const char* getHMMName( int hmmInd );
        int getInputVecSize();

        int getNumStates(int hmmInd) ;
        int getNumSuccessors(int hmmInd, int stateInd) ;
        int getSuccessor(int hmmInd, int stateInd, int sucInd) ;
        real getSuccessorLogProb(int hmmInd, int stateInd, int sucInd) ;

    private:
    	/*
    	 * Initialse HTK
    	 */
    	void InitialiseHTK();
    	/*
    	 * Parse simulated HTK command line arguments
    	 */
    	void ParseArgs();
    	/*
    	 * Tells HTK to load the HMMs
    	 */
    	void InitialiseHMMSet();
    	/*
    	 * Allocate memory for the HMM lookup table, names and transition probabilities
    	 */
    	void InitialiseHModels( bool removeInitialToFinalTransitions );
    	/*
    	 * Has HTK been initialised?
    	 */
    	bool isHTKinitialised;
    	/*
    	 * HTK Memory heap
    	 */
    	MemHeap hmmHeap;     /* Heap holds all hmm related info */
    	/*
    	 * Variable for recording the index of the current frame index as specified
    	 * by frame in the function call newFrame( int frame , const real *input )
    	 */
    	int currentFrameIndex;
    	/*
    	 * File related variables
    	 */
    	char *HTKCFG;		/* Filename of the HTK config file */
    	char *HTKMList;		/* Filename of the model list */
    	char *HTKMMF;       /* Filename of the MMF */
    	char * hmmDir;      /* directory to look for hmm def files */
    	char * hmmExt;      /* hmm def file extension */
    	/*
    	 * HTK's HMM storage
    	 */
    	HMMSet hSet;        /* current HMM set */
    	HMMSet *hset;       /* current HMM set */
    	int nHMMs;			/* number of physical HMMs in current set */
    	int maxStates;		/* max number of states in current HMM set */
    	int maxMixes;		/* max number of mixes in current HMM set */
    	/*
    	 * Lookup table for HMMs, their names and transition probabilities.
    	 */
    	int                 *HMMlookupTable;
    	StreamElem         **GMMlookupTable;
    	char               **HMMnames;
    	HModelsTransMatrix **HMMtransMat;  /* Array of pointers to transition matrices */
    	HModelsTransMatrix  *transMats;    /* Array of unique transition matrices */
    	/*
    	 * The HTK Observation structure that stores the frame
    	 * given by input in newFrame( int frame , const real *input )
    	 */
    	Observation currentFrameData;
    	/*
    	 * Cache area for HMM state probability calculations
    	 */
    	HModelCacheElement *stateProbCache;
    	/*
    	 * Returns pointer to corresponding TransMatrix of the specified HMM.
    	 * Will copy the transition matrix into transMats if it has not
    	 * been copied already.
    	 */
    	HModelsTransMatrix *getTransMatPtr( HLink hmm , bool removeInitialToFinalTransitions );
    	/*
    	 * Remove logical HMMs from the HMMSet
    	 */
    	Boolean noAlias;    /* set to zap all aliases in hmmlist */
    	void ZapAliases(void);

    };

}

#endif

