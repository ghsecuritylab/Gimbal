#include "CAN_com.h"

#define PITCH_ENC_FN              0
#define ROLL_ENC_FN               1
#define YAW_ENC_FN         			  2
#define ROLL_RPS_MCU_FN    			  3
#define PITCH_RPS_MCU_FN   			  4
#define YAW_RPS_MCU_FN     			  5
#define ROLL_RPL_MCU_NORMAL_FN    6
#define ROLL_RPL_MCU_HEAD_FN      7
#define PITCH_RPL_MCU_NORMAL_FN   8
#define PITCH_RPL_MCU_HEAD_FN     9
#define YAW_RPL_MCU_NORMAL_FN     10
#define YAW_RPL_MCU_HEAD_FN       11
#define ROLL_RS_MCU_FN            12
#define PITCH_RS_MCU_FN           13
#define YAW_RS_MCU_FN             14
#define ROLL_RL_MCU_FN            15
#define PITCH_RL_MCU_FN           16
#define YAW_RL_MCU_FN             17

static struct can_app_struct can_data; // �߳���������

const AP_Param::GroupInfo CAN::var_info[] =
{ 
    AP_GROUPINFO("ENC_X_BIAS", 0, CAN, enc_x_bias, 0),
    
    AP_GROUPINFO("ENC_Y_BIAS", 1, CAN, enc_y_bias, 0),
    
    AP_GROUPINFO("ENC_Z_BIAS", 2, CAN, enc_z_bias, 0),

    AP_GROUPEND
};

static rt_err_t can1ind(rt_device_t dev,  void *args, rt_int32_t hdr, rt_size_t size)
{
    rt_event_t pevent = (rt_event_t)args;
    rt_event_send(pevent, 1 << (hdr));
    return RT_EOK;
}

struct rt_can_filter_item filter1item[18] = 
{
		RT_CAN_FILTER_STD_LIST(PITCH_ENC,             RT_CAN_FIFO0,0,0,             0,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(ROLL_ENC,              RT_CAN_FIFO0,0,1,             1,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(YAW_ENC,         	    RT_CAN_FIFO0,0,2,             2,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(ROLL_RPS_MCU,    	    RT_CAN_FIFO0,0,3,             3,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(PITCH_RPS_MCU,   	    RT_CAN_FIFO0,1,0,             4,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(YAW_RPS_MCU,     	    RT_CAN_FIFO0,1,1,             5,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(ROLL_RPL_MCU_NORMAL,   RT_CAN_FIFO0,1,2,             6,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(ROLL_RPL_MCU_HEAD,     RT_CAN_FIFO0,1,3,             7,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(PITCH_RPL_MCU_NORMAL,  RT_CAN_FIFO0,2,0,             8,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(PITCH_RPL_MCU_HEAD,    RT_CAN_FIFO0,2,1,             9,  can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(YAW_RPL_MCU_NORMAL,    RT_CAN_FIFO0,2,2,             10, can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(YAW_RPL_MCU_HEAD,      RT_CAN_FIFO0,2,3,             11, can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(ROLL_RS_MCU,           RT_CAN_FIFO0,3,0,             12, can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(PITCH_RS_MCU,          RT_CAN_FIFO0,3,1,             13, can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(YAW_RS_MCU,            RT_CAN_FIFO0,3,2,             14, can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(ROLL_RL_MCU,           RT_CAN_FIFO0,3,3,             15, can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(PITCH_RL_MCU,          RT_CAN_FIFO0,4,0,             16, can1ind, (void*)&can_data.canevent),
		RT_CAN_FILTER_STD_LIST(YAW_RL_MCU,            RT_CAN_FIFO0,4,1,             17, can1ind, (void*)&can_data.canevent),
		
};

CAN::CAN():
  	_enc_fir_filter(ENC_COEF_LEN, enc_x_coef, enc_y_coef, enc_z_coef),
    _enc_sin_fir_filter(ENC_COEF_LEN, enc_x_coef, enc_y_coef, enc_z_coef),
		_enc_cos_fir_filter(ENC_COEF_LEN, enc_x_coef, enc_y_coef, enc_z_coef),
    _motor_speed_fir()
{
}


void CAN::start_init()
{
    struct rt_can_filter_config filter1;
    // can device parameter init
    filter1.count = 18;
    filter1.actived=1;
    filter1.fifobaseoff = 18;  // FIFO1������ƥ������ƫ��
    filter1.items = filter1item;
  
    can_data.name = "bxcan1";
    can_data.filter = &filter1;
    can_data.eventopt = RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR;
    
    candev = rt_device_find(can_data.name);    // �鿴�豸����
    RT_ASSERT(candev);
    rt_event_init(&can_data.canevent, can_data.name, RT_IPC_FLAG_FIFO);  // ��ʼ���¼�
    rt_device_open(candev, (RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX)); // ���豸����ʼ��
    rt_device_control(candev, RT_CAN_CMD_SET_FILTER, can_data.filter); // ����CAN���߹�����
    event_ops = can_data.filter->items;
	
    tx_msg.ide = 0;
    tx_msg.len = 8;
    tx_msg.rtr = 0;
		
    for(int i = 0; i < MAX_FRAMS; i++)
    {
        tx_msg_block[i].ide = 0;
        tx_msg_block[i].len = 8;
        tx_msg_block[i].rtr = 0;
        for(int j = 0; j < 8; j++)
        {
            tx_msg_block[i].data[j] = 0;
        }
    }
    
    tx_block.block_id = 7;
    tx_block.msg_total = 5;
    for(int i = 0; i < 2*tx_block.msg_total; i++)
    {
        tx_block.params[i].value = i;
    }
    
    //tx_block.param = (can_param *)rt_malloc(2*MAX_FRAMS*sizeof(can_param));
    for(int i = 0; i < 3; i++)
    {
        motors_params_for_serial[i].dev_addr = ROLL_MOTOR_ADDR;
        motors_params_for_serial[i].dev_addr = PITCH_MOTOR_ADDR;
        motors_params_for_serial[i].dev_addr = YAW_MOTOR_ADDR;
    }
    ax_motor_for_serial = 0;
		
		roll_rps_mcu_ok = false;
		pitch_rps_mcu_ok = false;
		yaw_rps_mcu_ok = false;
		roll_rpl_mcu_ok = false;
		pitch_rpl_mcu_ok = false;
		yaw_rpl_mcu_ok = false;
		
		motor_config_ready_to_send = false;
		motor_state_ready_to_send = false;
		
}

void CAN::write_test(rt_uint32_t id, int e)
{
	tx_msg.id = id;
	for(int i = 1; i < tx_msg.len; i++)
	{
		tx_msg.data[i] = 0;
	}
	tx_msg.data[0] = e;
	can_write(tx_msg);
}

int scop_enc_r, scop_enc_p, scop_enc_y;
void CAN::can_recv_data(void)
{
    // �ȴ������¼�
    if (rt_event_recv(&can_data.canevent,
        ((1 << event_ops[0].hdr)  |			  //PITCH_ENC_FN
         (1 << event_ops[1].hdr)  |           //ROLL_ENC_FN
         (1 << event_ops[2].hdr)  |           //YAW_ENC_FN
         (1 << event_ops[3].hdr)  |           //ROLL_RPS_MCU_FN
         (1 << event_ops[4].hdr)  |           //PITCH_RPS_MCU_FN
         (1 << event_ops[5].hdr)  |           //YAW_RPS_MCU_FN
         (1 << event_ops[6].hdr)  |           //ROLL_RPL_MCU_NORMAL_FN
         (1 << event_ops[7].hdr)  |           //ROLL_RPL_MCU_HEAD_FN 
         (1 << event_ops[8].hdr)  |           //PITCH_RPL_MCU_NORMAL_FN 
         (1 << event_ops[9].hdr)  |           //PITCH_RPL_MCU_HEAD_FN 
         (1 << event_ops[10].hdr) |           //YAW_RPL_MCU_NORMAL_FN   
         (1 << event_ops[11].hdr) |           //YAW_RPL_MCU_HEAD_FN  
         (1 << event_ops[12].hdr) |           //ROLL_RS_MCU_FN  
         (1 << event_ops[13].hdr) |           //PITCH_RS_MCU_FN  
         (1 << event_ops[14].hdr) |           //YAW_RS_MCU_FN 
         (1 << event_ops[15].hdr) |           //ROLL_RL_MCU_FN 
         (1 << event_ops[16].hdr) |           //PITCH_RL_MCU_FN       
         (1 << event_ops[17].hdr)),           //YAW_RL_MCU_FN   
        can_data.eventopt,
        RT_WAITING_FOREVER, &e) != RT_EOK){
        return;
    }
    //Ctrlstruct[0].angle_err = Ctrlstruct[0].angle_err + ahrsData.euler.x;
	//PITCH_ENC
    if (e & (1 << event_ops[0].hdr)){
        rx_msg.hdr = event_ops[0].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rt_memcpy(&motor_enc_raw.y,rx_msg.data,sizeof(int));
            motor_enc.y = circadjust(motor_enc_raw.y - enc_y_bias, 180);
					  scop_enc_p = motor_enc_raw.y*100;
        }
    }
	//ROLL_ENC
    if (e & (1 << event_ops[1].hdr)){
        rx_msg.hdr = event_ops[1].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rt_memcpy(&motor_enc_raw.x,rx_msg.data,sizeof(int));
            motor_enc.x = circadjust(motor_enc_raw.x - enc_x_bias, 180);
					  scop_enc_r = motor_enc_raw.x*100;
        }
    }
	//YAW_ENC
    if (e & (1 << event_ops[2].hdr)){
        rx_msg.hdr = event_ops[2].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rt_memcpy(&motor_enc_raw.z,rx_msg.data,sizeof(int));
            motor_enc.z = circadjust(motor_enc_raw.z - enc_z_bias, 180);
					  scop_enc_y = motor_enc_raw.z*100;
        }
    }
    //ROLL_RPS_MCU
    if (e & (1 << event_ops[3].hdr)){
        rx_msg.hdr = event_ops[3].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rx_param.dev_addr = ROLL_MOTOR_ADDR;
            rt_memcpy(&rx_param, &rx_msg.data, 2*sizeof(float));
            motors_params_for_serial[0].params[rx_param.param_id] = rx_param.value;
					  roll_rps_mcu_ok = true;
					  // Motor config read complete
					  if(rx_param.param_id == 14) motor_config_ready_to_send = true;
					  if(rx_param.param_id == 28) motor_state_ready_to_send = true;
        }
    }
    //PITCH_RPS_MCU
	if (e & (1 << event_ops[4].hdr)){
        rx_msg.hdr = event_ops[4].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rx_param.dev_addr = PITCH_MOTOR_ADDR;
            rt_memcpy(&rx_param, &rx_msg.data, 2*sizeof(float));
            motors_params_for_serial[1].params[rx_param.param_id] = rx_param.value;
					  pitch_rps_mcu_ok = true;
					  // Motor config read complete
					  if(rx_param.param_id == 14) motor_config_ready_to_send = true;
					  if(rx_param.param_id == 28) motor_state_ready_to_send = true;
        }
    }
    //YAW_RPS_MCU
	if (e & (1 << event_ops[5].hdr)){
        rx_msg.hdr = event_ops[5].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rx_param.dev_addr = YAW_MOTOR_ADDR;
            rt_memcpy(&rx_param, &rx_msg.data, 2*sizeof(float));
            motors_params_for_serial[2].params[rx_param.param_id] = rx_param.value;
					  yaw_rps_mcu_ok = true;
						// Motor config read complete
					  if(rx_param.param_id == 14) motor_config_ready_to_send = true;
					  if(rx_param.param_id == 28) motor_state_ready_to_send = true;
        }
    }
	//ROLL_RPL_MCU_NORMAL
	if (e & (1 << event_ops[6].hdr)){
        rx_msg.hdr = event_ops[6].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rt_memcpy((float*)(&rx_block.params[2*rx_block.msg_cnt].value), &rx_msg.data[0], sizeof(float));
            rt_memcpy((float*)(&rx_block.params[2*rx_block.msg_cnt+1].value), &rx_msg.data[4], sizeof(float));
            rx_block.msg_cnt++;		
            if(rx_block.msg_cnt >= rx_block.msg_total) roll_rpl_mcu_ok = true;
        }
    }		
    //ROLL_RPL_MCU_HEAD
	if (e & (1 << event_ops[7].hdr)){
        rx_msg.hdr = event_ops[7].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rx_block.dev_addr = ROLL_MOTOR_ADDR;
            rt_memcpy(&rx_block, &rx_msg.data, sizeof(float));
            rx_block.msg_cnt = 0;
        }
    }		
    //PITCH_RPL_MCU_NORMAL 
	if (e & (1 << event_ops[8].hdr)){
        rx_msg.hdr = event_ops[8].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rt_memcpy((float*)(&rx_block.params[2*rx_block.msg_cnt].value), &rx_msg.data, sizeof(float));
            rt_memcpy((float*)(&rx_block.params[2*rx_block.msg_cnt+1].value), &rx_msg.data[4], sizeof(float));
            rx_block.msg_cnt++;	
            if(rx_block.msg_cnt >= rx_block.msg_total) pitch_rpl_mcu_ok = true;
        }
    }
    //PITCH_RPL_MCU_HEAD 
    if (e & (1 << event_ops[9].hdr)){
        rx_msg.hdr = event_ops[9].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rx_block.dev_addr = PITCH_MOTOR_ADDR;
            rt_memcpy(&rx_block, &rx_msg.data, sizeof(float));
            rx_block.msg_cnt = 0;
        }
    }
    //YAW_RPL_MCU_NORMAL 
	if (e & (1 << event_ops[10].hdr)){
        rx_msg.hdr = event_ops[10].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rt_memcpy((float*)(&rx_block.params[2*rx_block.msg_cnt].value), &rx_msg.data, sizeof(float));
            rt_memcpy((float*)(&rx_block.params[2*rx_block.msg_cnt+1].value), &rx_msg.data[4], sizeof(float));
            rx_block.msg_cnt++;	
            if(rx_block.msg_cnt >= rx_block.msg_total) yaw_rpl_mcu_ok = true;
        }
    }		
    //YAW_RPL_MCU_HEAD 
	if (e & (1 << event_ops[11].hdr)){
        rx_msg.hdr = event_ops[11].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            rx_block.dev_addr = YAW_MOTOR_ADDR;
            rt_memcpy(&rx_block, &rx_msg.data, sizeof(float));
            rx_block.msg_cnt = 0;
        }
    }
    //ROLL_RS_MCU 
	if (e & (1 << event_ops[12].hdr)){
        rx_msg.hdr = event_ops[12].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            
            //write_test(ROLL_RS_MCU, 12);
            //write_param(ROLL_MOTOR_ADDR, 0, 1);
        }
    }		
	//PITCH_RS_MCU 
	if (e & (1 << event_ops[13].hdr)){
        rx_msg.hdr = event_ops[13].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            
            //write_test(PITCH_RS_MCU, 12);
            //write_param(PITCH_MOTOR_ADDR, 0, 2);
        }
    }
    //YAW_RS_MCU 
	if (e & (1 << event_ops[14].hdr)){
        rx_msg.hdr = event_ops[14].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            
            //write_test(YAW_RS_MCU, 12);
            //write_param(YAW_MOTOR_ADDR, 0, 3);
        }
    }
    //ROLL_RL_MCU 
	if (e & (1 << event_ops[15].hdr)){
        rx_msg.hdr = event_ops[15].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
					
            //write_test(ROLL_RL_MCU, 12);					
            //write_block(ROLL_MOTOR_ADDR, tx_block);				
        }
    }
    //PITCH_RL_MCU 
	if (e & (1 << event_ops[16].hdr)){
        rx_msg.hdr = event_ops[16].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            //write_test(PITCH_RL_MCU, 12);				
            //write_block(PITCH_MOTOR_ADDR, tx_block);
        }
    }		
	//YAW_RL_MCU 
	if (e & (1 << event_ops[17].hdr)){
        rx_msg.hdr = event_ops[17].hdr;
        while (rt_device_read(candev, 0, &rx_msg, sizeof(rx_msg)) == sizeof(rx_msg)){
            //write_test(YAW_RL_MCU, 12);			
            //write_block(YAW_MOTOR_ADDR, tx_block);
        }
    }		
}

void CAN::update()
{
	can_recv_data();	
}



rt_size_t CAN::can_write(struct rt_can_msg msg)
{
    return rt_device_write(candev, 0, &msg, sizeof(msg));
}

float limit_loop(float x, float lim)
{
	if(x > lim)
	{
		while(x > lim) x -= 2*lim;
	}
	if(x <= -lim)
	{
		while(x <= -lim) x += 2*lim;
	}
	return x;
}

int scop_can_sx, scop_can_sy, scop_can_sz, scop_can_cx, scop_can_cy, scop_can_cz;
void CAN::get_enc_trifun(Vector3f &enc)
{
	Vector3f angle, angle_sin, angle_cos;
	Vector3f angle_sin_fir, angle_cos_fir;
	angle = motor_enc / 57.29577951308232f;
	angle_sin(sinf(angle.x), sinf(angle.y), sinf(angle.z));
	angle_cos(cosf(angle.x), cosf(angle.y), cosf(angle.z));
	angle_sin_fir = _enc_sin_fir_filter.update(angle_sin);
	angle_cos_fir = _enc_cos_fir_filter.update(angle_cos);
	
	angle(atan2f(angle_sin_fir.x, angle_cos_fir.x), atan2f(angle_sin_fir.y, angle_cos_fir.y), atan2f(angle_sin_fir.z, angle_cos_fir.z));
	enc = angle;
	
	scop_can_sx = enc.x * 5729;
	scop_can_sy = enc.y * 5729;
	scop_can_sz = enc.z * 5729;
	scop_can_cx = angle_cos_fir.x * 1000;
	scop_can_cy = angle_cos_fir.y * 1000;
	scop_can_cz = angle_cos_fir.z * 1000;
}

void correct_angle_deg(Vector3f *angle, Vector3f *last_angle, float lim)
{
	if(angle->x - last_angle->x > lim)
	{
		while(angle->x - last_angle->x > lim) angle->x -= 2*lim;
	}
	else if(angle->x - last_angle->x < -lim)
	{
		while(angle->x - last_angle->x < -lim) angle->x += 2*lim;
	}
	
	if(angle->y - last_angle->y > lim)
	{
		while(angle->y - last_angle->y > lim) angle->y -= 2*lim;
	}
	else if(angle->y - last_angle->y < -lim)
	{
		while(angle->y - last_angle->y < -lim) angle->y += 2*lim;
	}
	
	if(angle->z - last_angle->z > lim)
	{
		while(angle->z - last_angle->z > lim) angle->z -= 2*lim;
	}
	else if(angle->z - last_angle->z < -lim)
	{
		while(angle->z - last_angle->z < -lim) angle->z += 2*lim;
	}
}

Vector3f angle_diff(Vector3f a, Vector3f la)
{
	Vector3f d = a - la;
	if(d.x > PI/2) d.x -= 2*PI;
	else if(d.x < -PI/2) d.x += 2*PI;
	if(d.y > PI/2) d.y -= 2*PI;
	else if(d.y < -PI/2) d.y += 2*PI;
	if(d.z > PI/2) d.z -= 2*PI;
	else if(d.z < -PI/2) d.z += 2*PI;
	
	return d;
}

int scop_can_angle_x, scop_can_angle_y,  scop_can_angle_z;
int scop_can_angle_fir_x, scop_can_angle_fir_y, scop_can_angle_fir_z;
int js_spzr, js_spzf;
void CAN::get_enc(Vector3f &enc, Vector3f &enc_speed)
{
	static Vector3f last_angle, angle, speed, angle_out, last_angle_out;
	

  last_angle = angle;
	angle = motor_enc/57.29577951308232f;
//	correct_angle_deg(&angle, &last_angle, 180.0f);
	last_angle_out = angle_out;
//	angle_out = _enc_fir_filter.update(angle);
//	enc(limit_loop(angle_out.x, PI), limit_loop(angle_out.y, PI), limit_loop(angle_out.z, PI));
	enc = angle;
//	speed = angle_diff(angle, last_angle);
//	enc_speed = _motor_speed_fir.update(speed);
	enc_speed(0, 0, 0);
	
	js_spzr = speed.z * 1000000;
	js_spzf = enc_speed.z * 1000000;

//	
//	scop_can_angle_x = motor_enc.x * 100.0f;
//	scop_can_angle_y = motor_enc.y * 100.0f;
//	scop_can_angle_z = motor_enc.z * 100.0f;
//	
//	scop_can_angle_fir_x = enc.x * 5729.57795f;
//	scop_can_angle_fir_y = enc.y * 5729.57795f;
//	scop_can_angle_fir_z = enc.z * 5729.57795f;

}

void CAN::encoder_zero_calibration(void)
{
	enc_x_bias = motor_enc_raw.x;
	enc_y_bias = motor_enc_raw.y;
	enc_z_bias = motor_enc_raw.z;
	// save params
}

short map2short(float x)
{
	return (short)(100.0f*x);
}


int scop_can_r_mot, scop_can_p_mot, scop_can_y_mot;
int scop_can_rs_mot, scop_can_ps_mot, scop_can_ys_mot;
void CAN::send_control_rpy(float r_mot, float p_mot, float y_mot)
{
	short mot[3];
	rt_uint32_t level;
	level = rt_hw_interrupt_disable();
	mot[0] = map2short(r_mot);
	mot[1] = map2short(p_mot);
	mot[2] = map2short(y_mot);
	scop_can_r_mot = r_mot*1000;
	scop_can_p_mot = p_mot*1000;
	scop_can_y_mot = y_mot*1000;
	scop_can_rs_mot = mot[0]*10;
	scop_can_ps_mot = mot[1]*10;
	scop_can_ys_mot = mot[2]*10;
	tx_msg.id = RPY_CTRL;
	tx_msg.priv = 0;
	rt_memcpy(&tx_msg.data, mot, 3*sizeof(short));
	can_write(tx_msg);
  rt_hw_interrupt_enable(level);
}

void CAN::send_control_rp(float roll_mot, float pitch_mot)
{
    rt_uint32_t level;
    level = rt_hw_interrupt_disable();	
    tx_msg.id = RP_CTRL;
    tx_msg.priv = 0;
    rt_memcpy(&tx_msg.data[0], &roll_mot, sizeof(float));
    rt_memcpy(&tx_msg.data[4], &pitch_mot, sizeof(float));
    can_write(tx_msg);
    rt_hw_interrupt_enable(level);
}

void CAN::send_control_y(float yaw_motor)
{
	rt_uint32_t level;
	level = rt_hw_interrupt_disable();	
	tx_msg.id = Y_CTRL;
	tx_msg.priv = 0;
	rt_memcpy(&tx_msg.data, &yaw_motor, sizeof(float));
	can_write(tx_msg);
	rt_hw_interrupt_enable(level);
}

void CAN::write_param(rt_uint32_t dst_addr, rt_uint32_t param_addr, float value)
{
	rt_uint32_t level;
	level = rt_hw_interrupt_disable();
	switch(dst_addr)
	{
		case ROLL_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_WS_ROLL;
		  break;
		case PITCH_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_WS_PITCH;
		  break;
		case YAW_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_WS_YAW;
		  break;
		case TRIPOD_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_WS_TRPMOT;
		  break;
		default:
			return;
	}
	tx_msg_block[0].priv = 2;
	rt_memcpy(&tx_msg_block[0].data, &param_addr, sizeof(rt_uint32_t));
	rt_memcpy(&tx_msg_block[0].data[4], &value, sizeof(float));
	rt_hw_interrupt_enable(level);
	can_write(tx_msg_block[0]);
}

void CAN::read_param(rt_uint32_t dst_addr, rt_uint32_t param_addr, float param_value)
{
	float value;
	value = param_value;
	switch(dst_addr)
	{
		case ROLL_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_RS_ROLL;
		  break;
		case PITCH_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_RS_PITCH;
		  break;
		case YAW_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_RS_YAW;
		  break;
		default:
			return;
	}
	tx_msg_block[0].priv = 2;
	rt_memcpy(&tx_msg_block[0].data, &param_addr, sizeof(rt_uint32_t));
	rt_memcpy(&tx_msg_block[0].data[4], &value, sizeof(float));
	can_write(tx_msg_block[0]);
}
		
void CAN::write_block(rt_uint32_t dst_addr, can_block block)
{
	uint32_t normal_id;
	switch(dst_addr)
	{
		case ROLL_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_WL_ROLL_HEAD;
		  normal_id = MCU_WL_ROLL_NORMAL;
		  break;
		case PITCH_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_WL_PITCH_HEAD;
		  normal_id = MCU_WL_PITCH_NORMAL;
		  break;
		case YAW_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_WL_YAW_HEAD;
		  normal_id = MCU_WL_YAW_NORMAL;
		  break;
		default:
			return;
	}
	tx_msg_block[0].priv = 3;
	rt_memcpy(&tx_msg_block[0].data, &block, 2*sizeof(rt_uint32_t));
	tx_msg_block[0].id = normal_id;
	for(block.msg_cnt = 0; block.msg_cnt < block.msg_total; block.msg_cnt++)
	{
		tx_msg_block[block.msg_cnt].id = normal_id;
		rt_memcpy(&tx_msg_block[block.msg_cnt].data[0], (void*)(&block.params[2*block.msg_cnt].value), sizeof(float));
		rt_memcpy(&tx_msg_block[block.msg_cnt].data[4], (void*)(&block.params[2*block.msg_cnt + 1].value), sizeof(float));
	}
	//rt_device_write(candev, 0, &tx_msg_block, block.msg_total*sizeof(tx_msg_block[0]));
	for(int i = 0; i < block.msg_total; i++)
	{
		can_write(tx_msg_block[i]);
		rt_thread_delay(2);
	}
	
}
		
void CAN::read_block(rt_uint32_t dst_addr, rt_uint32_t block_id)
{
	switch(dst_addr)
	{
		case ROLL_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_RL_ROLL;
		  break;
		case PITCH_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_RL_PITCH;
		  break;
		case YAW_MOTOR_ADDR:
			tx_msg_block[0].id = MCU_RL_YAW;
		  break;
		default:
			return;
	}
	tx_msg_block[0].priv = 3;
	rt_memcpy(&tx_msg_block[0].data, &block_id, sizeof(rt_uint32_t));
	can_write(tx_msg_block[0]);
}
