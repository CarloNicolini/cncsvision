// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

#ifndef _DBayesianModel_H
#define _DBayesianModel_H

#include "Distribution.h"
using namespace std;


/**
* \class DBayesianModel
* \ingroup Statistic
* \brief DBayesianModel3D is a specific model composed of 4 multivariate normal distributions product.
* The model is:
*
* \f[
* P(\mathbf{G} | \mathbf{a}) = \int  \limits_{\Omega , \Omega_0 } N_1(0,\Sigma_g) N_2 (0,\Sigma_{\omega} ) N_3(0,\Sigma_{\omega_0}) N_4(\mathbf{a}, E ) d \Omega d \Omega_0
* \f]
* where the following equalities hold:
* \f$ \Omega = (\omega_x, \omega_y, \omega_z) \f$, \f$ \Omega_0 = (\omega_{0_x}, \omega_{0_y}, \omega_{0_z} ) \f$, \f$ \mathbf{a}=(a_1,a_2,a_3,a_4,a_5,a_6) \f$ 
* \f[
* a_1 = \omega_{0_y} + \omega_y + \epsilon_1
* \f]
* \f[
* a_2 = -\omega_{0_y} g_x - \omega_{0_z} + \epsilon_2
* \f]
* \f[
* a_3 = -\omega_{0_y} g_y - \omega_z + \epsilon_3
* \f]
* \f[
* a_4 = \omega_{0_x} + \omega_x + \epsilon_4
* \f]
* \f[
* a_5 = -\omega_{0_x} g_x + \omega_z + \epsilon_5
* \f]
* \f[
* a_6 = -\omega_{0_x} +g_y \omega_{0_z} + \epsilon_6
* \f]
**/

class DBayesianModel : public Distribution
{
	private:
		unsigned int dimension;
		bool ready;
		double gx, gy,omegax, omegay, omegaz, omega0x, omega0y, omega0z;
		double a[6], ma[6],e[6];
		
		double sigmaOmega0x,sigmaOmega0y,sigmaOmega0z,sigmaOmegax,sigmaOmegay,sigmaOmegaz,sigmaGx,sigmaGy;
		double mOmegax,mOmegay,mOmegaz,mOmega0x,mOmega0y,mOmega0z;
		
	        double denominator;
	public:
	        DBayesianModel(){};
		DBayesianModel(unsigned int );
		~DBayesianModel();

		// Set the parameters for the complete bayesian model of Fulvio
		void finalize();
		void setSigmaG(double sigmaGx, double sigmaGy); 
		void setSigmaOmega0( double sigmaOmega0x, double sigmaOmega0y, double sigmaOmega0z);
		void setSigmaOmega( double sigmaOmegax, double sigmaOmegay, double sigmaOmegaz);
		void setEpsilons(double,double,double,double,double,double);
		void setGxGy(double gx, double gy);
		void setOmegaMean(double , double ,double );
		void setOmega0Mean(double , double ,double );
		void setAMean(double, double, double, double, double, double );
		
	protected:
		inline void body(const VectorXd &x) ;

};

#endif
