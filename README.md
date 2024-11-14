# Play_selected_video
Combine the random AviPlayer and the video selection UI into a video player that allows the user to select which video will be played next.

**Code needs fixing**                                                            
Error message states:                                                            
15:02:38.200 -> Touchpad pressed: x=269, y=308                                                            
15:02:38.326 -> avi_open(/root/avi320x480/BP07.avi)                                                            
15:02:39.222 -> AVI avi_total_frames: 13142, 320 x 480 @ 15.00 fps, format: cvidGuru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.
15:02:39.222 ->                                                             
15:02:39.222 -> Core  1 register dump:                                                            
15:02:39.222 -> PC      : 0x4204d50a  PS      : 0x00060c30  A0      : 0x8204dbb2  A1      : 0x3fced170  
15:02:39.222 -> A2      : 0x3fcec74c  A3      : 0x3fcf1718  A4      : 0x3fc98ea0  A5      : 0x00000000  
15:02:39.222 -> A6      : 0xa5a5a5a5  A7      : 0x3fc98ea8  A8      : 0x3fced2a0  A9      : 0x3fced280  
15:02:39.222 -> A10     : 0x00000210  A11     : 0x00000000  A12     : 0x00000000  A13     : 0x00000140  
15:02:39.222 -> A14     : 0x000001e0  A15     : 0x00000000  SAR     : 0x0000000c  EXCCAUSE: 0x0000001c  
15:02:39.222 -> EXCVADDR: 0xa5a5a62d  LBEG    : 0x40056f08  LEND    : 0x40056f12  LCOUNT  : 0x00000000  
15:02:39.222 ->                                                             
15:02:39.222 ->                                                             
15:02:39.222 -> Backtrace: 0x4204d507:0x3fced170 0x4204dbaf:0x3fced1a0 0x4204e4e0:0x3fced1d0 0x42032580:0x3fced1f0 0x420849d5:0x3fced210 0x4208454b:0x3fced230 0x420843a1:0x3fced260 0x4203136f:0x3fced280 0x42031395:0x3fced2a0 0x42003706:0x3fced2c0 0x42003909:0x3fced380 0x42006179:0x3fced3c0 0x42006245:0x3fced3e0 0x42006bca:0x3fced420 0x42007890:0x3fced450 0x420079e6:0x3fced470 0x42026335:0x3fced4b0 0x420263d9:0x3fced4d0 0x42005a88:0x3fced4f0 0x4037fff6:0x3fced510
15:02:39.222 ->                                                             
15:02:39.222 ->                                                             
                                                            
                                                            
OR                                                            
                                                            
                                                            
15:15:46.818 -> Touchpad pressed: x=233, y=308                                                            
15:15:46.897 -> avi_open(/root/avi320x480/BP03.avi)                                                            
15:15:47.804 -> AVI avi_total_frames: 13139, 320 x 480 @ 15.00 fps, format: cvid, estimateBufferSize: 61440, ESP.getFreeHeap(): 128808
15:15:47.804 -> Audio chaGuru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.
15:15:47.804 ->                                                             
15:15:47.804 -> Core  1 register dump:                                                            
15:15:47.804 -> PC      : 0x4204d50a  PS      : 0x00060430  A0      : 0x8204dbb2  A1      : 0x3fced170  
15:15:47.804 -> A2      : 0x3fcec74c  A3      : 0x3fcf1718  A4      : 0x3fc98ea0  A5      : 0x00000000  
15:15:47.804 -> A6      : 0xa5a5a5a5  A7      : 0x3fc98ea8  A8      : 0x3fced2a0  A9      : 0x3fced280  
15:15:47.804 -> A10     : 0x00000210  A11     : 0x00000000  A12     : 0x00000000  A13     : 0x00000140  
15:15:47.804 -> A14     : 0x000001e0  A15     : 0x00000000  SAR     : 0x0000000c  EXCCAUSE: 0x0000001c  
15:15:47.804 -> EXCVADDR: 0xa5a5a62d  LBEG    : 0x40056f08  LEND    : 0x40056f12  LCOUNT  : 0x00000000  
15:15:47.804 ->                                                             
15:15:47.804 ->                                                             
15:15:47.804 -> Backtrace: 0x4204d507:0x3fced170 0x4204dbaf:0x3fced1a0 0x4204e4e0:0x3fced1d0 0x42032580:0x3fced1f0 0x420849d5:0x3fced210 0x4208454b:0x3fced230 0x420843a1:0x3fced260 0x4203136f:0x3fced280 0x42031395:0x3fced2a0 0x42003706:0x3fced2c0 0x42003909:0x3fced380 0x42006179:0x3fced3c0 0x42006245:0x3fced3e0 0x42006bca:0x3fced420 0x42007890:0x3fced450 0x420079e6:0x3fced470 0x42026335:0x3fced4b0 0x420263d9:0x3fced4d0 0x42005a88:0x3fced4f0 0x4037fff6:0x3fced510
15:15:47.804 ->                                                             
15:15:47.804 ->                                                             
                                                            
Note that the two Core 1 register dumps are identical apart from PS : 0x00060c30 in the first one and  PS : 0x00060430 in the second one.
