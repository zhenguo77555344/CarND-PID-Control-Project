#include "PID.h"
#include<vector>
using namespace std;

/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

PID::PID() {



}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  /**
   * Initialize PID coefficients (and errors, if needed)
   */
  m_Kp = Kp;
  m_Ki = Ki;
  m_Kd = Kd;
}

void PID::UpdateError(double cte,double pre_cte) {
  /**
   * TODO: Update PID errors based on cte.
   */
  
  double diff_cte;
  double int_cte;
  diff_cte = cte - pre_cte;
  int_cte +=cte;
  pre_cte=cte;
  
  m_steer_value = -m_Kp * cte - m_Kd * diff_cte - m_Ki * int_cte;

  //m_steer_value = -m_Kp * cte - m_Kd * diff_cte - 0 * int_cte;

  m_error = cte*cte;

}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  vector<double> para(0,3);
  para.push_back(m_Kp);
  para.push_back(m_Ki);
  para.push_back(m_Kd);

  return para.size();  // TODO: Add your total error calc here!
}