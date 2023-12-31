#include <Encoder.h>
Encoder fl_enc(fl_a, fl_b);
Encoder fr_enc(fr_a, fr_b);
Encoder bl_enc(bl_a, bl_b);
Encoder br_enc(br_a, br_b);

//when using intterupt pins (as you should for pin A on all)
//the encoder is accurate otherwise it is too slow

//--------------------------------------//
//             Encoders                 //
//--------------------------------------//

void setupEncoders(){
  fl_enc.write(0);
  fr_enc.write(0);
  bl_enc.write(0);
  br_enc.write(0);
  
  fl_pos = 0;
  fr_pos = 0;
  br_pos = 0;
  bl_pos = 0;
  }

void readEncoders(){
  fl_pos = fl_enc.read();
  fr_pos = fr_enc.read();
  br_pos = br_enc.read();
  bl_pos = bl_enc.read();
  }
