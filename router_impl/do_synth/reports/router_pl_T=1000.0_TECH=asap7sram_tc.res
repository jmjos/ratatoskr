 
****************************************
Report : resources
Design : router_pl
Version: K-2015.06-SP4
Date   : Wed Nov 28 15:40:25 2018
****************************************


No resource sharing information to report.

No implementations to report

No resource sharing information to report.
 
****************************************
Design : arbiter_7_1_1_1_1_DXYU
****************************************

No implementations to report
 
****************************************
Design : switch_allocator_7_DXYU
****************************************
Resource Sharing Report for design switch_allocator_7_DXYU in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/arbiter/switch_allocator.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r436     | DW01_inc     | width=5    |               | add_219_2            |
| r438     | DW_mod_tc    | a_width=5  |               | mod_219              |
|          |              | b_width=32 |               |                      |
| r440     | DW01_add     | width=6    |               | add_1_root_add_219_2_G2 |
| r442     | DW_mod_tc    | a_width=6  |               | mod_219_G2           |
|          |              | b_width=32 |               |                      |
| r444     | DW01_add     | width=6    |               | add_1_root_add_219_2_G3 |
| r446     | DW_mod_tc    | a_width=6  |               | mod_219_G3           |
|          |              | b_width=32 |               |                      |
| r448     | DW01_add     | width=6    |               | add_1_root_add_219_2_G4 |
| r450     | DW_mod_tc    | a_width=6  |               | mod_219_G4           |
|          |              | b_width=32 |               |                      |
| r452     | DW01_add     | width=6    |               | add_1_root_add_219_2_G5 |
| r454     | DW_mod_tc    | a_width=6  |               | mod_219_G5           |
|          |              | b_width=32 |               |                      |
| r456     | DW01_add     | width=6    |               | add_1_root_add_219_2_G6 |
| r458     | DW_mod_tc    | a_width=6  |               | mod_219_G6           |
|          |              | b_width=32 |               |                      |
| r460     | DW01_add     | width=6    |               | add_1_root_add_219_2_G7 |
| r462     | DW_mod_tc    | a_width=6  |               | mod_219_G7           |
|          |              | b_width=32 |               |                      |
===============================================================================


Implementation Report
===============================================================================
|                    |                  | Current            | Set            |
| Cell               | Module           | Implementation     | Implementation |
===============================================================================
| mod_219            | DW_mod_tc        | rpl                |                |
| mod_219_G2         | DW_mod_tc        | cla                |                |
| mod_219_G3         | DW_mod_tc        | cla                |                |
| mod_219_G4         | DW_mod_tc        | cla                |                |
| mod_219_G5         | DW_mod_tc        | cla                |                |
| mod_219_G6         | DW_mod_tc        | cla                |                |
| mod_219_G7         | DW_mod_tc        | cla                |                |
===============================================================================

 
****************************************
Design : switch_allocator_7_DXYU_DW_mod_tc_11
****************************************

Resource Sharing Report for design DW_mod_tc_a_width5_b_width32

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r89      | DW_div       | a_width=5  |               | u_div                |
|          |              | b_width=32 |               |                      |
|          |              | tc_mode=1  |               |                      |
|          |              | rem_mode=0 |               |                      |
===============================================================================

 
****************************************
Design : switch_allocator_7_DXYU_DW_mod_tc_5
****************************************

Resource Sharing Report for design DW_mod_tc_a_width6_b_width32

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r89      | DW_div       | a_width=6  |               | u_div                |
|          |              | b_width=32 |               |                      |
|          |              | tc_mode=1  |               |                      |
|          |              | rem_mode=0 |               |                      |
===============================================================================

 
****************************************
Design : switch_allocator_7_DXYU_DW_mod_tc_4
****************************************

Resource Sharing Report for design DW_mod_tc_a_width6_b_width32

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r89      | DW_div       | a_width=6  |               | u_div                |
|          |              | b_width=32 |               |                      |
|          |              | tc_mode=1  |               |                      |
|          |              | rem_mode=0 |               |                      |
===============================================================================

 
****************************************
Design : switch_allocator_7_DXYU_DW_mod_tc_3
****************************************

Resource Sharing Report for design DW_mod_tc_a_width6_b_width32

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r89      | DW_div       | a_width=6  |               | u_div                |
|          |              | b_width=32 |               |                      |
|          |              | tc_mode=1  |               |                      |
|          |              | rem_mode=0 |               |                      |
===============================================================================

 
****************************************
Design : switch_allocator_7_DXYU_DW_mod_tc_2
****************************************

Resource Sharing Report for design DW_mod_tc_a_width6_b_width32

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r89      | DW_div       | a_width=6  |               | u_div                |
|          |              | b_width=32 |               |                      |
|          |              | tc_mode=1  |               |                      |
|          |              | rem_mode=0 |               |                      |
===============================================================================

 
****************************************
Design : switch_allocator_7_DXYU_DW_mod_tc_1
****************************************

Resource Sharing Report for design DW_mod_tc_a_width6_b_width32

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r89      | DW_div       | a_width=6  |               | u_div                |
|          |              | b_width=32 |               |                      |
|          |              | tc_mode=1  |               |                      |
|          |              | rem_mode=0 |               |                      |
===============================================================================

 
****************************************
Design : switch_allocator_7_DXYU_DW_mod_tc_0
****************************************

Resource Sharing Report for design DW_mod_tc_a_width6_b_width32

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r89      | DW_div       | a_width=6  |               | u_div                |
|          |              | b_width=32 |               |                      |
|          |              | tc_mode=1  |               |                      |
|          |              | rem_mode=0 |               |                      |
===============================================================================


No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_7
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_8
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_6
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_9
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_7
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_8
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_10
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_9
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_10
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_11
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_11
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : credit_count_single_vc_depth_out2_12
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_12
****************************************

No implementations to report
 
****************************************
Design : credit_count_single_vc_depth_out2_0
****************************************
Resource Sharing Report for design credit_count_single_vc_depth_out2_0 in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/arbiter/credit_count_single.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r300     | DW01_inc     | width=2    |               | add_52               |
| r302     | DW01_dec     | width=2    |               | sub_54               |
| r304     | DW_cmp       | width=2    |               | lt_gt_59             |
===============================================================================


No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_13
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_14
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_15
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_16
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_17
****************************************

No implementations to report
 
****************************************
Design : rr_arbiter_no_delay_CNT2_0
****************************************
Resource Sharing Report for design rr_arbiter_no_delay_CNT2_0 in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/arbiter/rr_arbiter_no_delay.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r299     | DW01_dec     | width=2    |               | sub_72               |
| r301     | DW01_sub     | width=2    |               | sub_add_74_b0        |
| r303     | DW01_sub     | width=2    |               | sub_add_76_b0        |
===============================================================================


No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_allocator_7_1_1_1_1_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_output_allocator_port_num7_vc_num_out2_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT6_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : header_arbiter_and_decoder_1_1_1_7_6_2_1_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : routing_calc_Xis1_Yis1_Zis1_rout_algoDXYU_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : dxyu_routing_Xis1_Yis1_Zis1_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_output_allocator_port_num7_vc_num_out2_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT6_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : header_arbiter_and_decoder_1_1_1_7_5_2_1_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : routing_calc_Xis1_Yis1_Zis1_rout_algoDXYU_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : dxyu_routing_Xis1_Yis1_Zis1_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_output_allocator_port_num7_vc_num_out2_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT6_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : header_arbiter_and_decoder_1_1_1_7_4_2_1_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_6
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : routing_calc_Xis1_Yis1_Zis1_rout_algoDXYU_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : dxyu_routing_Xis1_Yis1_Zis1_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_output_allocator_port_num7_vc_num_out2_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT6_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : header_arbiter_and_decoder_1_1_1_7_3_2_1_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_7
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_8
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : routing_calc_Xis1_Yis1_Zis1_rout_algoDXYU_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : dxyu_routing_Xis1_Yis1_Zis1_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_output_allocator_port_num7_vc_num_out2_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT6_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : header_arbiter_and_decoder_1_1_1_7_2_2_1_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_9
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_10
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : routing_calc_Xis1_Yis1_Zis1_rout_algoDXYU_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : dxyu_routing_Xis1_Yis1_Zis1_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_5
****************************************

No implementations to report
 
****************************************
Design : vc_output_allocator_port_num7_vc_num_out2_0
****************************************
Resource Sharing Report for design vc_output_allocator_port_num7_vc_num_out2_0
        in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/arbiter/vc_output_allocator.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r310     | DW01_inc     | width=2    |               | add_122              |
===============================================================================


No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT6_6
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : header_arbiter_and_decoder_1_1_1_7_1_2_1_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_11
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : seq_packet_counter_1_12
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : routing_calc_Xis1_Yis1_Zis1_rout_algoDXYU_6
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : dxyu_routing_Xis1_Yis1_Zis1_6
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : rr_arbiter_no_delay_CNT2_6
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_output_allocator_port_num7_vc_num_out1
****************************************

No implementations to report
 
****************************************
Design : rr_arbiter_no_delay_CNT6_0
****************************************
Resource Sharing Report for design rr_arbiter_no_delay_CNT6_0 in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/arbiter/rr_arbiter_no_delay.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r299     | DW01_dec     | width=6    |               | sub_72               |
| r301     | DW01_sub     | width=6    |               | sub_add_74_b0        |
| r303     | DW01_sub     | width=6    |               | sub_add_76_b0        |
===============================================================================


No implementations to report

No resource sharing information to report.
 
****************************************
Design : header_arbiter_and_decoder_1_1_1_7_0_1_1_DXYU
****************************************

No implementations to report
 
****************************************
Design : seq_packet_counter_1_0
****************************************
Resource Sharing Report for design seq_packet_counter_1_0 in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/arbiter/seq_packet_counter.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r293     | DW01_dec     | width=4    |               | sub_52               |
===============================================================================


No implementations to report

No resource sharing information to report.
 
****************************************
Design : routing_calc_Xis1_Yis1_Zis1_rout_algoDXYU_0
****************************************

No implementations to report
 
****************************************
Design : dxyu_routing_Xis1_Yis1_Zis1_0
****************************************
Resource Sharing Report for design dxyu_routing_Xis1_Yis1_Zis1_0 in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/routing_algos/dxyu_routing.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r304     | DW_cmp       | width=2    |               | gt_52                |
| r306     | DW_cmp       | width=2    |               | gt_56                |
| r308     | DW_cmp       | width=2    |               | gt_58                |
===============================================================================


No implementations to report

No resource sharing information to report.
 
****************************************
Design : output_register_vc_num2_vc_num_out2_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : output_register_vc_num2_vc_num_out2_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : output_register_vc_num2_vc_num_out2_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : output_register_vc_num2_vc_num_out2_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : output_register_vc_num2_vc_num_out2_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : output_register_vc_num2_vc_num_out2_0
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : output_register_vc_num1_vc_num_out1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : crossbar_7_DXYU
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_input_buffer_2_0000000200000002_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_1
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_input_buffer_2_0000000200000002_2
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_input_buffer_2_0000000200000002_3
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_6
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_input_buffer_2_0000000200000002_4
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_7
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_8
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_input_buffer_2_0000000200000002_5
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_9
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_10
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_input_buffer_2_0000000200000002_0
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_11
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : fifo_buff_depth2_12
****************************************

No implementations to report

No resource sharing information to report.
 
****************************************
Design : vc_input_buffer_1_0000000200000002
****************************************

No implementations to report
 
****************************************
Design : fifo_buff_depth2_0
****************************************
Resource Sharing Report for design fifo_buff_depth2_0 in file
        /usrf06/home/agids/usr/bamberg/ongoing_work/noc_3D/rtl/input_buffer/fifo.vhd

===============================================================================
|          |              |            | Contained     |                      |
| Resource | Module       | Parameters | Resources     | Contained Operations |
===============================================================================
| r364     | DW_mod_uns   | a_width=1  |               | mod_62               |
|          |              | b_width=31 |               |                      |
| r366     | DW_mod_uns   | a_width=1  |               | mod_65               |
|          |              | b_width=31 |               |                      |
| r368     | DW_mod_uns   | a_width=1  |               | mod_79               |
|          |              | b_width=31 |               |                      |
===============================================================================


No implementations to report
1
