#define sgn(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))
#define wheel_seperation .557784
#define wheel_base .808482
#define wheel_radius 0.155575
#define wheel_steering_y_offset 0.00
float steering_track = (wheel_seperation - 2*wheel_steering_y_offset);


void four_ws_control(float x, float y, float z, int mode){
       float vel[4] = {0, 0, 0, 0}; 
       float pos[4] = {0, 0, 0, 0}; 
        float ang = 0.0;
        float V = 0.0;
        float vel_steerring_offset = z * wheel_steering_y_offset;
        int sign = sgn(x);

  switch(mode){
    case 1: // opposite phase
        vel[0] = sign*sqrt(sq(x - z*steering_track/2) + sq(z*wheel_base/2)) - vel_steerring_offset;
        vel[1] = sign*sqrt(sq(x + z*steering_track/2) + sq(z*wheel_base/2)) + vel_steerring_offset; 
        vel[2] = sign*sqrt(sq(x - z*steering_track/2) + sq(z*wheel_base/2)) - vel_steerring_offset; 
        vel[3] = sign*sqrt(sq(x + z*steering_track/2) + sq(z*wheel_base/2)) + vel_steerring_offset; 
      
        if ((2*x + z*steering_track) == 0.0){
            pos[0] = 0.0 ;
            pos[1] = 0.0;
            pos[2] = 0.0;
            pos[3] = 0.0;
        }//end if
        else{
            pos[0] = atan(z*wheel_base/(2*x + z*steering_track));
            pos[1] = atan(z*wheel_base/(2*x - z*steering_track));
            pos[2] = -pos[0];
            pos[3] = -pos[1];
        }//End else
      break;

    case 2: // in phase
        ang = 0;
        V = sqrt(sq(x)+sq(y));


        if(x != 0) { ang = y / x; }
        else { ang = 0; }

        pos[0] = atan(ang);
        pos[1] = atan(ang);
        pos[2] = pos[0];
        pos[3] = pos[1];
        
        vel[0] = sign*V;
        vel[1] = sign*V;
        vel[2] = sign*V;
        vel[3] = sign*V;
      break;
    case 3: // pivot

        pos[0] = -atan(wheel_base/steering_track);
        pos[1] = atan(wheel_base/steering_track);
        pos[2] = atan(wheel_base/steering_track);
        pos[3] = -atan(wheel_base/steering_track);
        
        vel[0] = -z;
        vel[1] = z;
        vel[2] = vel[0];
        vel[3] = vel[1];
      break;
    default:
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        vel[3] = 0.0;
  }//end switch
    for (int i = 0; i < 4; i++) {
      lin_vels[i] = vel[i];
      pos_angs[i] = pos[i];
    }
}//end function
