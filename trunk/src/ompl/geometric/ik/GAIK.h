/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Ioan Sucan */

#ifndef OMPL_GEOMETRIC_PLANNERS_IK_GAIK_
#define OMPL_GEOMETRIC_PLANNERS_IK_GAIK_

#include "ompl/base/SpaceInformation.h"
#include "ompl/base/GoalRegion.h"
#include "ompl/geometric/ik/HCIK.h"
#include "ompl/util/Console.h"

namespace ompl
{
    
    namespace geometric
    {
	
	/**
	   @anchor GAIK
	   
	   @par Short description
	   
	   GAIK does inverse kinematics, but makes sure the produced
	   goal states are in fact valid.       
	   
	   @par External documentation
	*/

	/** \brief Inverse Kinematics with Genetic Algorithms */
	class GAIK
	{
	public:
	    
	    /** \brief Construct an instance of a genetic algorithm for inverse kinematics given the space information to search within */
	    GAIK(const base::SpaceInformationPtr &si) : hcik_(si), msg_("GAIK")
	    {
		si_ = si;
		maxDistance_ = si_->getStateValidityCheckingResolution() * 100.0;
		poolSize_ = 80;
		poolExpansion_ = 100;
		hcik_.setMaxImproveSteps(3);
		setValidityCheck(true);
		sampler_ = si_->allocStateSampler();
	    }
	    
	    ~GAIK(void)
	    {
	    }

	    /** \brief Find a state that fits the request */
	    bool solve(double solveTime, const base::GoalRegion &goal, base::State *result,
		       const std::vector<base::State*> &hint = std::vector<base::State*>());
	    
	    /** \brief Set the number of steps to perform when using hill climbing to improve an individual in the population */
	    void setMaxImproveSteps(unsigned int maxSteps)
	    {
		hcik_.setMaxImproveSteps(maxSteps);
	    }

	    /** \brief Get the number of steps to perform when using hill climbing to improve an individual in the population */
	    unsigned int getMaxImproveSteps(void) const
	    {
		return hcik_.getMaxImproveSteps();
	    }
	    
	    /** \brief Set the state validity flag; if this is false, states are not checked for validity */
	    void setValidityCheck(bool valid)
	    {
		checkValidity_ = valid;
		hcik_.setValidityCheck(valid);
	    }

	    /** \brief Get the state validity flag; if this is false, states are not checked for validity */
	    bool getValidityCheck(void) const
	    {
		return checkValidity_;
	    }
	    
	    /** \brief Set the number of individuals in the population */
	    void setPoolSize(unsigned int size)
	    {
		poolSize_ = size;
	    }

	    /** \brief Get the number number of individuals in the population */
	    unsigned int getPoolSize(void) const
	    {
		return poolSize_;
	    }
	    
	    /** \brief Set the number of individuals to add to the population in each generation */
	    void setPoolExpansionSize(unsigned int size)
	    {
		poolExpansion_ = size;
	    }
	    
	    /** \brief Get the number of individuals to add to the population in each generation */
	    unsigned int getPoolExpansionSize(void) const
	    {
		return poolExpansion_;
	    }
	    
	    /** \brief Set the range (distance) to be used when sampling around a state */
	    void setRange(double distance)
	    {
		maxDistance_ = distance;
	    }
	    
	    /** \brief Get the range GAIK is using */
	    double getRange(void) const
	    {
		return maxDistance_;
	    }
	    
	private:
	    
	    /** \brief Use hill climbing to attempt to get a state closer to the goal */
	    bool tryToImprove(const base::GoalRegion &goal, base::State *state, double distance);
	    
	    /** \brief Return true if the state is to be considered valid. This function always returns true if checking of validity is disabled. */
	    bool valid(const base::State *state) const
	    {
		return checkValidity_ ? si_->isValid(state) : true;
	    }
	    
	    
	    struct Individual
	    {
		base::State *state;
		double       distance;
		bool         valid;
	    };
	    
	    struct IndividualSort
	    {
		bool operator()(const Individual& a, const Individual& b)
		{
		    if (a.valid == b.valid)
			return a.distance < b.distance;
		    return a.valid;
		}
	    };
	    
	    HCIK                                         hcik_;
	    base::StateSamplerPtr                        sampler_;
	    base::SpaceInformationPtr                    si_;	
	    unsigned int                                 poolSize_;
	    unsigned int                                 poolExpansion_;
	    bool                                         checkValidity_;	
	    
	    double                                       maxDistance_;	

	    msg::Interface                               msg_;
	};
	
    }
}

#endif
