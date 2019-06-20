#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

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
  /**
   * TODO: Initialize the pid variable.
   */
 
 

  //Controler.Init(Kp,Ki,Kd,Error);
  
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
        Kp = 0.2;
        Ki = 0;
        Kd = 3.0;
        Error = 0;



        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;


          double angle_err;
          double diff_cte;

          /**
           * TODO: Calculate steering value here, remember the steering value is [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          

          //Controler.UpdateError(cte,pre_cte);
          //pre_cte =cte;
          //angle_err = Controler.TotalError();
          //std::cout<<"para.size()"<<angle_err<<std::endl;

          diff_cte = cte -pre_cte;
          int_cte = int_cte +cte;
          steer_value = -Kp * cte - Kd * diff_cte - Ki * int_cte;
          pre_cte = cte;
          //std::cout<<"PID:---->"<<Kp<<","<<Ki<<","<<Kd<<std::endl;

          //it=it+1;
          // DEBUG
          //std::cout <<"Iteration: "<<it<< " CTE: " << cte << " Steering Value: " << steer_value <<" Speed:"<<speed<<" Steering_angle: "<<angle << std::endl;
          std::cout<<"PID--->"<<Kp<<","<<Ki<<","<<Kd<<std::endl;
          std::cout << cte << ","<<diff_cte<<","<<int_cte<<","<<steer_value<<","<<speed<<","<<angle << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
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