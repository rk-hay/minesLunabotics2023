#include <Encoder.h>
Encoder fl_enc(fl_a, fl_b);
Encoder fr_enc(fr_a, fr_b);
Encoder bl_enc(bl_a, bl_b);
Encoder br_enc(br_a, br_b);

//when using intterupt pins (as you should for pin A on all)
//the encoder is accurate otherwise it is too slow
//num of encs per rev = 
//--------------------------------------//
//             Encoders                 //
//--------------------------------------//

void setupEncoders(){
  
  
  fl_enc.write(0);
  fr_enc.write(0);
  bl_enc.write(0);
  br_enc.write(0);
  
  fl_enc_pos = 0;
  fr_enc_pos = 0;
  br_enc_pos = 0;
  bl_enc_pos = 0;
  }

void readEncoders(){
  fl_enc_pos = fl_enc.read();
  fr_enc_pos = -fr_enc.read();
  br_enc_pos = -br_enc.read();
  bl_enc_pos = bl_enc.read();

  if (fl_enc_pos > 5281 || fl_enc_pos < -5281){
    int sig = (fl_enc_pos > 0) ? 1:-1;
    fl_rotations = fl_rotations + sig;
    fl_enc.write(0);
    }
  if (fr_enc_pos > 5281 || fr_enc_pos < -5281){
    int sig = (fr_enc_pos > 0) ? 1:-1;
    fr_rotations = fr_rotations + sig;
    fr_enc.write(0);
    }
  
  if (bl_enc_pos > 5281 || bl_enc_pos < -5281){
    int sig = (bl_enc_pos > 0) ? 1:-1;
    bl_rotations = bl_rotations + sig;
    bl_enc.write(0);
    }
  if (br_enc_pos > 5281 || br_enc_pos < -5281){
    int sig = (br_enc_pos > 0) ? 1:-1;
    br_rotations = br_rotations + sig;
    br_enc.write(0);
    }
  }
  
