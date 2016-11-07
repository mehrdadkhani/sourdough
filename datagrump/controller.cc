#include <iostream>
#include "controller.hh"
#include "timestamp.hh"
#include <vector>
using namespace std;
/*int A = 1;
int M = 2;
bool ack_arrival_flag = 0;
bool timeout_flag = 0;
unsigned int the_window_size = 50;*/
/*double delta_thr;
unsigned int This_RTT=1;
unsigned int prev_window_size = 0;
unsigned int prev_RTT = 1;
double  delta_ack_t;
unsigned int This_timestamp;
unsigned int prev_timestamp = 0;
*/
bool AIMD_tout_flag = 0;
bool ack_flag = 1;
uint64_t AIMD_ack_time = 0;
uint64_t AIMD_ack_time_prev = 0;
int RTT = 100;
int prev_RTT = 100;
int RTT_min = 1000;
double send_delay = 0;
double ack_delay = 0;
double prev_ack_delay=0;
double prev_send_delay = 0;
uint64_t prev_send_time=0;
uint64_t prev_ack_time=0;
uint64_t prev_ack_time_2 =0;
uint64_t prev_seq = 0;
uint64_t prev_seq_2 =0;
int64_t gholi = 0;
time_t timev;
unsigned int temp;
unsigned int temp2=0;
double BW = 0;
double W = 1;
double dwind =0;
double prev_BW = 0;
double next_BW = 0;
bool congestion = 0;
int dummyi=0;
bool keys=0;
unsigned int pre_window = 0;
double RTT_avg = 100;
/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ),A(100),M(0.6),ack_arrival_flag(0),timeout_flag(0),the_window_size(10),delta(0),exp_next_seq(1)	
{}
/* Get current window size, in datagrams */
unsigned int Controller::window_size( void ){
//================================
// CONSTANT WINDOW SIZE SCENARIO
//================================
/*	the_window_size = 10;
		return the_window_size;

*/


//================================
// AIMD SCENARIO
//================================
/*	AIMD_tout_flag =((AIMD_ack_time > (AIMD_ack_time_prev + 15)) ? 1:0);
	AIMD_ack_time_prev = AIMD_ack_time;
	if (ack_flag){
		ack_flag = 0;
		the_window_size = the_window_size + 1;
	}
	
	if (AIMD_tout_flag){
		AIMD_tout_flag = 0;
		the_window_size = (unsigned int)((double)the_window_size * 0.5);
	}
	cout << the_window_size << endl;
	return the_window_size;
*/

//===============================
//DELAY TRIGGERED SCHEME
//===============================
/*
if (ack_flag){		
	if (RTT < 80){
		the_window_size = the_window_size + 2;
	}
	if (RTT >= 80 && RTT<110){
		the_window_size = (unsigned int)((double)the_window_size -1);
	}
	if (RTT >= 110){
		the_window_size =(unsigned int)((double)the_window_size * 0.5);
	}
	if (the_window_size == 0){
		the_window_size = 1;
	}
	ack_flag = 0;
	}
	return the_window_size;
*/

//===============================
// CONTEST
//===============================

	RTT_min = ((RTT < RTT_min) ? RTT : RTT_min);
	if (ack_flag){
		ack_flag = 0;
		if ((send_delay <= ack_delay) && (ack_delay > 0)){	//BW Limited
			BW = ((double)(1.0 /(double)ack_delay));	
			next_BW = BW + (BW - prev_BW) * send_delay / ack_delay;
			next_BW = ((next_BW > 0) ? next_BW : 0);
			prev_BW = BW;
//			W = ( (double)RTT_min * next_BW);
			W = (double)RTT_min * next_BW * ((RTT_avg > 65)? (65/RTT_avg):1);
			cout <<"1:"<<RTT_avg << endl;
			keys = 1;
		}
		if (send_delay > ack_delay && keys){
			W = W + 10;
			keys = 0;	
				
		//	if (RTT_avg < 100 && RTT_avg >= 20){
                  //              the_window_size = the_window_size +1;}
		//	if (RTT_avg >= 80 && RTT_avg < 110){
                  //              the_window_size = the_window_size;}
			cout << "2:" <<RTT_avg  << endl;
		}


		if (RTT_avg > 110){W = W * 0.9;}

	}
/*
	cout << "ack_delay: " << ack_delay
        << " send_delay: " << send_delay 
        << " RTT: " << RTT 
        << " W: " << the_window_size  
	<< " BW: " << BW << endl;

	cout << (RTT - RTT_min) << endl;
*/	the_window_size = (unsigned int)W;
	 return the_window_size;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
	ack_flag = 1;
	AIMD_ack_time = timestamp_ack_received;
	RTT = (timestamp_ack_received - send_timestamp_acked);
	if (timestamp_ack_received - prev_ack_time >= 40)
	{
		send_delay = ((double)(send_timestamp_acked - prev_send_time)/(double)(sequence_number_acked - prev_seq));
		ack_delay = ((double)(timestamp_ack_received - prev_ack_time)/(double)(sequence_number_acked - prev_seq));
		RTT_avg = ((double)(timestamp_ack_received - send_timestamp_acked)/(double)(sequence_number_acked - prev_seq_2));
		prev_send_time =send_timestamp_acked;
		prev_ack_time = timestamp_ack_received;
		prev_seq = sequence_number_acked;
	} 
	if(timestamp_ack_received - prev_ack_time_2 >= 10){	
                RTT_avg = ((double)(timestamp_ack_received - send_timestamp_acked)/(double)(sequence_number_acked - prev_seq_2));
		prev_seq_2 = sequence_number_acked;
		prev_ack_time_2 = timestamp_ack_received;
	}
if ( debug_ ){
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 20; /* timeout of one second */
}
