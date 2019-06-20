#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

#include<numeric>
#include<vector>
#include <algorithm>
#include <stdlib.h>

// for convenience
using nlohmann::json;
using std::string;
using namespace std;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID Controler;

  Controler.Init(0.2,0.0,3.0);//Initialize the pid variable.

  h.onMessage([&Controler](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        static double pre_cte;
        static double int_cte;
        double Kp,Ki,Kd;
        double Error;
        Kp = 0.2; //0.2
        Ki = 0.0;   //0
        Kd = 3.0; //3
        Error = 0;

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;
          double throttle_value;

          double angle_err;
          double diff_cte;

          diff_cte = cte -pre_cte;
          int_cte = int_cte +cte;
          Controler.UpdateError(cte,pre_cte);

          steer_value = Controler.GetSteerValue();

          pre_cte = cte;

          std::cout << cte << ","<<diff_cte<<","<<int_cte<<","<<steer_value<<","<<speed<<","<<angle << std::endl;

          double tolerance =0.0001;
          double arr_dp[] = {1,1,1};
          double arr_p[] = {0,0,0};
          double sum_dp;
          vector<double> dp(&arr_dp[0],&arr_dp[3]);
          vector<double> p(&arr_p[0],&arr_p[3]);

          sum_dp = accumulate(dp.begin(),dp.end(),0);
          //std::cout <<"sum_dp:-->"<<sum_dp<<std::endl;


          /*
          while(sum_dp > tolerance){
            
            for(int i = 0;i<p.size();i++){
              //cout<<"i = "<<i<<endl;
              p.at(i) +=dp.at(i);
              
              if(abs(cte)>=1.0){
                throttle_value =0.0;            
              }
              else{
                Controler.UpdateError(cte,pre_cte);
                throttle_value =0.3;


              }
                                   

            }
          //steer_value =0.02;
          }
          */

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          //msgJson["throttle"] = throttle_value;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;//"steering_angle":-3.28192,"throttle":0.3
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}