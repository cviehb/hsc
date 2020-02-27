#ifndef SLAVE_H_
#define SLAVE_H_

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

/* Slave module: */
// no process -> will only be called by initiator therefore no context-switch
struct Slave : sc_module{
public:
	//target socket using default settings
    tlm_utils::simple_target_socket<Slave> socket;

	// CTOR
    SC_HAS_PROCESS(Slave);
    Slave(sc_module_name instance) : socket("bus_rw"), LATENCY(10, SC_NS){
        // socket.register registers func/pros to be called fom master
		// register callback for incoming b_transport interface method call
        socket.register_b_transport(this, &Slave::b_transport);
		// register callback for incoming get_direct_mem_pt interface method call
		socket.register_get_direct_mem_ptr(this, &Slave::get_direct_mem_ptr);
		// register callback for incoming transport_dbg interface method call
		socket.register_transport_dbg(this, &Slave::transport_dbg);

		// Initialize memory with random data
		for (int i = 0; i < SIZE; i++)
			mem[i] = 0xAA000000 | (rand() % 256);
    }


    /*********************************************************
		TLM 2 blocking transport method
	*********************************************************/
    virtual void b_transport(tlm::tlm_generic_payload & trans, sc_time & delay){
        // read parameters of transaction object
		tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address()/4;
        //unsigned int *  ptr = reinterpret_cast<unsigned int *>(trans.get_data_ptr());
		unsigned char *  ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();
        unsigned char *  byt = trans.get_byte_enable_ptr();
        unsigned int     wid = trans.get_streaming_width();

		// decode transaction and check parameters
        if(adr >= sc_dt::uint64(SIZE)){
			trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");
			return;
        }

		if(byt != 0){
			trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
			return;
		}

		if(len > 4 || wid < len){
			trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
			return;
		}

		// read or write data -> can be copied by memcpy or direct access via pointer
        if(cmd == tlm::TLM_READ_COMMAND){
			// *ptr = mem[adr];				// in this case we need the typecast when reading the parameter
            memcpy(ptr, &mem[adr], len);
        }else if(cmd == tlm::TLM_WRITE_COMMAND){
            // mem[adr] = *ptr;
			memcpy(&mem[adr], ptr, len);
        }


		// Set DMI hint to indicated that DMI is supported
		trans.set_dmi_allowed(true);

        // finaly set the response status attribute of the gerneric payload to
		// indicate successful and complete transaction
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }


	/*********************************************************
		TLM 2: method to get point for direct memory access
	**********************************************************/
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload & trans,
									tlm::tlm_dmi & dmi_data){
		// Permit read and write access
		dmi_data.allow_read_write();

		// Set other details of DMI region
		dmi_data.set_dmi_ptr( reinterpret_cast<unsigned char*>( &mem[0] ) );
		dmi_data.set_start_address( 0 );
		dmi_data.set_end_address( SIZE-1 );
		dmi_data.set_read_latency( LATENCY );
		dmi_data.set_write_latency( LATENCY );

		return true;
	}

	/*********************************************************
		TLM 2: blocking transport debug method
	**********************************************************/
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload & trans){
		tlm::tlm_command cmd = trans.get_command();
		sc_dt::uint64 adr    = trans.get_address()/4;
		unsigned char* ptr   = trans.get_data_ptr();
		unsigned int len     = trans.get_data_length();

		unsigned int num_bytes = (len < SIZE - adr) ? len : SIZE - adr;

		if(cmd == tlm::TLM_READ_COMMAND){
			memcpy(ptr, &mem[adr], num_bytes);
		}else if(cmd == tlm::TLM_WRITE_COMMAND){
			memcpy(&mem[adr], ptr, num_bytes);
		}

		return num_bytes;
	}

private:
	// size of memory
    enum { SIZE = 256 };
	// latency for direct memory access
	const sc_time LATENCY;
	// memory
    int mem[SIZE];
};

#endif
