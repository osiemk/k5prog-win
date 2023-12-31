
/*
 * Use at your own risk
 *
 * Uses modified comm routines from
 *   Jacek Lipkowski <sq5bpf@lipkowski.org> 
 *   https://github.com/sq5bpf/k5prog
 *
 * This program is licensed under the GNU GENERAL PUBLIC LICENSE v3
 * License text avaliable at: http://www.gnu.org/copyleft/gpl.html
 */

/*
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef Unit1H
#define Unit1H

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include "CGAUGES.h"

#include <vector>
#include <stdint.h>

#include "SerialPort.h"
#include "HighResolutionTick.h"
#include "CriticalSection.h"

#define WM_INIT_GUI          (WM_USER + 100)
#define WM_CONNECT           (WM_USER + 101)
#define WM_DISCONNECT        (WM_USER + 102)

#define ARRAY_SIZE(array)    (sizeof(array) / sizeof(array[0]))

// ******************************************************************************

#define MODE_NONE                            0            //
#define MODE_READ                            1            //
#define MODE_WRITE                           2            //
#define MODE_WRITE_MOST                      3            //
#define MODE_WRITE_ALL                       4            //
#define MODE_FLASH_DEBUG                     5            //
#define MODE_FLASH                           6            //

#define UVK5_HELLO_TRIES                     2            //

#define UVK5_CONFIG_SIZE                     0x00001d00   // 7424
#define UVK5_CALIB_SIZE                      0x00000200   // 512
#define UVK5_MAX_CONFIG_SIZE                 0x00002000   // 8192
#define UVK5_CONFIG_BLOCKSIZE                128          //

#define UVK5_FLASH_SIZE                      0x0000f000   // 61440
#define UVK5_MAX_FLASH_SIZE                  0x00010000   // 65536   the bootloader is in F000 to FFFF
#define UVK5_FLASH_BLOCKSIZE                 256          //

#define DEFAULT_SERIAL_SPEED                 38400        //
//#define DEFAULT_FILE_NAME                    "k5_config.raw"
//#define DEFAULT_FLASH_NAME                   "k5_flash.raw"

// ******************************************************************************

#pragma option push
#pragma warn -8027

struct k5_command
{
	uint8_t *cmd;
	int      len;
	uint8_t *obfuscated_cmd;
	int      obfuscated_len;
	uint16_t crc_clear;
	int      crc_ok;
};

// ******************************************************************************

// my calibration data UV-K5(8) 1
//
// 001E00  eeprom start address
//
// offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
// -------------------------------------------------------
// 000000  0A 4B 53 56 59 5C 5F 62 64 66 FF FF FF FF FF FF
// 000010  05 46 50 53 56 59 5C 5F 62 64 FF FF FF FF FF FF
// 000020  5A 2D 29 26 23 20 1D 1A 17 14 FF FF FF FF FF FF
// 000030  64 30 2D 29 26 23 20 1D 1A 17 FF FF FF FF FF FF
// 000040  5A 14 11 0E 0B 08 03 02 02 02 FF FF FF FF FF FF
// 000050  64 11 0E 0B 08 05 05 04 04 04 FF FF FF FF FF FF
//
// 000060  32 68 6B 6E 6F 72 75 77 79 7B FF FF FF FF FF FF
// 000070  28 64 67 6A 6C 6E 71 73 76 78 FF FF FF FF FF FF
// 000080  41 32 2D 28 24 21 1E 1A 17 16 FF FF FF FF FF FF
// 000090  46 37 32 2D 28 25 22 1E 1B 19 FF FF FF FF FF FF
// 0000A0  5A 19 0F 0A 09 08 07 06 05 04 FF FF FF FF FF FF
// 0000B0  64 1E 14 0F 0D 0C 0B 0A 09 08 FF FF FF FF FF FF
//
//                                -70dB -65dB -60dB -55dB
//         110   120   130   140   180   190   200   210
// 0000C0  6E 00 78 00 82 00 8C 00 B4 00 BE 00 C8 00 D2 00
//
// 0000D0  32 32 32 64 64 64 8C 8C 8C FF FF FF FF FF FF FF
// 0000E0  32 32 32 64 64 64 8C 8C 8C FF FF FF FF FF FF FF
// 0000F0  5F 5F 5F 69 69 69 91 91 8F FF FF FF FF FF FF FF
// 000100  32 32 32 64 64 64 8C 8C 8C FF FF FF FF FF FF FF
// 000110  5A 5A 5A 64 64 64 82 82 82 FF FF FF FF FF FF FF
// 000120  5A 5A 5A 64 64 64 8F 91 8A FF FF FF FF FF FF FF
// 000130  32 32 32 64 64 64 8C 8C 8C FF FF FF FF FF FF FF
// 000140  DE 04 FA 06 45 07 5E 07 C5 07 FC 08 FF FF FF FF
// 000150  1E 00 32 00 46 00 5A 00 6E 00 82 00 96 00 AA 00
// 000160  C8 00 E6 00 FF FF FF FF 14 00 28 00 3C 00 50 00
// 000170  64 00 78 00 8C 00 A0 00 BE 00 DC 00 FF FF FF FF
// 000180  03 08 0E 13 18 FF FF FF 04 00 46 00 50 00 2C 0E
// 000190  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
// 0001A0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
// 0001B0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
// 0001C0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
// 0001D0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
// 0001E0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
// 0001F0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF

#pragma pack(push, 1)
// starts at eeprom address 0x01E00
//typedef struct __attribute__((packed, aligned(1)))
typedef struct
{
	struct                                      // 0x0000
	{
		uint8_t open_rssi_thresh[10];
		uint8_t unused1[6];

		uint8_t close_rssi_thresh[10];
		uint8_t unused2[6];

		uint8_t open_noise_thresh[10];
		uint8_t unused3[6];

		uint8_t close_noise_thresh[10];
		uint8_t unused4[6];

		uint8_t open_glitch_thresh[10];
		uint8_t unused5[6];

		uint8_t close_glitch_thresh[10];
		uint8_t unused6[6];
	} squelch[2];

	uint16_t unknown1[4];                      // 0x00C0
	uint16_t rssi[4];                          // 0x00C8

	struct                                     // 0x00D0
	{
		uint8_t low_tx_pwr[3];
		uint8_t mid_tx_pwr[3];
		uint8_t high_tx_pwr[3];
		uint8_t unused[7];
	} band_setting[7];

	uint16_t battery[6];                       // 0x0140
	uint8_t  unused1[4];

	struct                                     // 0x0150
	{
		uint16_t threshold[10];
		uint8_t  unused[4];
	} vox[2];

	uint8_t  mic_gain_dB2[5];                  // 0x0180
	uint8_t  unused4[3];

	int16_t  bk4819_xtal_freq_low;             // 0x0188
	uint16_t unknown2;
	uint16_t unknown3;
	uint8_t  volume_gain;
	uint8_t  dac_gain;

} t_calibration;
#pragma pack(pop)

// ******************************************************************************

typedef void __fastcall (__closure *mainForm_threadProcess)();

class CThread : public TThread
{
	private:
		mainForm_threadProcess m_process;
		bool                   m_sync;
		DWORD                  m_sleep_ms;
		HANDLE                 m_mutex;

	protected:
		void __fastcall Execute()
		{
			DWORD res;
			while (!Terminated)
			{
				res = WAIT_FAILED;
				if (m_mutex)
				{
					res = WaitForSingleObject(m_mutex, m_sleep_ms);
					if (res == WAIT_TIMEOUT)
					{
					}
					else
					if (res == WAIT_OBJECT_0)
					{
						ReleaseMutex(m_mutex);
					}
				}
				else
				{
					Sleep(m_sleep_ms);
				}

				if (m_process != NULL)
				{
					if (!m_sync)
						m_process();
					else
						Synchronize(m_process);
				}
			}
			ReturnValue = 0;
		}

	public:
		__fastcall CThread(mainForm_threadProcess process, TThreadPriority priority, DWORD sleep_ms, bool start, bool sync) : TThread(!start)
		{
			m_sleep_ms = sleep_ms;
			m_process  = process;
			m_sync     = sync;

			FreeOnTerminate = false;
			Priority        = priority;

			m_mutex = CreateMutex(NULL, TRUE, NULL);
		}

		virtual __fastcall ~CThread()
		{
			m_process = NULL;

			if (m_mutex)
			{
				WaitForSingleObject(m_mutex, 100);		// wait for upto 100ms
				CloseHandle(m_mutex);
				m_mutex = NULL;
			}
		}

		__property bool Sync     = {read = m_sync,     write = m_sync};
		__property DWORD SleepMS = {read = m_sleep_ms, write = m_sleep_ms};
		__property HANDLE Mutex  = {read = m_mutex};
};

// ******************************************************************************

class TForm1 : public TForm
{
__published:
	TStatusBar *StatusBar1;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	TTimer *Timer1;
	TPanel *Panel1;
	TMemo *Memo1;
	TPanel *Panel2;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *SerialPortComboBox;
	TComboBox *SerialSpeedComboBox;
	TButton *ClearButton;
	TButton *ReadConfigButton;
	TButton *WriteFirmwareButton;
	TTrackBar *VerboseTrackBar;
	TButton *WriteConfigButton;
	TButton *ReadADCButton;
	TButton *ReadRSSIButton;
	TButton *ReadCalibrationButton;
	TSaveDialog *SaveDialog2;
	TCGauge *CGauge1;
	TButton *WriteCalibrationButton;
	TOpenDialog *OpenDialog2;
	TOpenDialog *OpenDialog3;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall SerialPortComboBoxDropDown(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall ClearButtonClick(TObject *Sender);
	void __fastcall VerboseTrackBarChange(TObject *Sender);
	void __fastcall ReadConfigButtonClick(TObject *Sender);
	void __fastcall WriteFirmwareButtonClick(TObject *Sender);
	void __fastcall WriteConfigButtonClick(TObject *Sender);
	void __fastcall SerialPortComboBoxChange(TObject *Sender);
	void __fastcall ReadADCButtonClick(TObject *Sender);
	void __fastcall ReadRSSIButtonClick(TObject *Sender);
	void __fastcall SerialPortComboBoxSelect(TObject *Sender);
	void __fastcall SerialSpeedComboBoxSelect(TObject *Sender);
	void __fastcall ReadCalibrationButtonClick(TObject *Sender);
	void __fastcall StatusBar1Resize(TObject *Sender);
	void __fastcall WriteCalibrationButtonClick(TObject *Sender);

private:

	String                m_ini_filename;

	int                   m_screen_width;
	int                   m_screen_height;
	SYSTEM_INFO           m_system_info;

	CCriticalSectionObj   m_thread_cs;
	CThread              *m_thread;

	String                m_loadfile_name;
	std::vector <uint8_t> m_loadfile_data;

	String                m_bootloader_ver;
	String                m_firmware_ver;
	bool                  m_has_custom_AES_key;
	bool                  m_is_in_lock_screen;
	uint8_t               m_challenge[4];

	int                   m_verbose;

//	uint8_t               m_config[UVK5_MAX_CONFIG_SIZE];
	uint8_t               m_config[UVK5_CONFIG_SIZE];
	uint8_t               m_calib[UVK5_CALIB_SIZE];

	std::vector < std::vector <uint8_t> > m_rx_packet_queue;

	struct
	{
		String                port_name;
		CSerialPort           port;
		std::vector <uint8_t> rx_buffer;
		volatile uint32_t     rx_buffer_wr;
		CHighResolutionTick   rx_timer;
	} m_serial;

	void __fastcall loadSettings();
	void __fastcall saveSettings();

	int    __fastcall saveFile(String filename, const uint8_t *data, const size_t size);
	size_t __fastcall loadFile(String filename);

	HANDLE __fastcall openBleService();

	void __fastcall comboBoxAutoWidth(TComboBox *comboBox);
	void __fastcall updateSerialPortCombo();

	void __fastcall disconnect();
	bool __fastcall connect(const bool clear_memo = true);

	void __fastcall threadProcess();

	void __fastcall clearRxPacket0();
	void __fastcall clearRxPacketQueue();

	std::vector <String> __fastcall stringSplit(String s, String param);

	void     __fastcall make_CRC16_table();
	uint16_t __fastcall crc16(const uint8_t *data, const int size);

	void __fastcall k5_hex_dump2(const struct k5_command *cmd, const bool tx);
	void __fastcall k5_destroy_struct(struct k5_command *cmd);
	void __fastcall k5_hdump(const uint8_t *buf, const int len);
	void __fastcall k5_hex_dump(const struct k5_command *cmd);
	void __fastcall k5_xor_firmware(uint8_t *data, const int len);
	void __fastcall k5_xor_payload(uint8_t *data, const int len);
	int  __fastcall k5_obfuscate(struct k5_command *cmd);
	int  __fastcall k5_deobfuscate(struct k5_command *cmd);
	int  __fastcall k5_send_cmd(struct k5_command *cmd);
	int  __fastcall k5_send_buf(const uint8_t *buf, const int len);
	int  __fastcall k5_read_eeprom(uint8_t *buf, const int len, const int offset);
	int  __fastcall k5_write_eeprom(uint8_t *buf, const int len, const int offset);
	int  __fastcall k5_wait_flash_message();
	int  __fastcall k5_send_flash_version_message(const char *ver);
//	int  __fastcall k5_read_flash(uint8_t *buf, const int len, const int offset);
	int  __fastcall k5_write_flash(const uint8_t *buf, const int len, const int offset, const int firmware_size);
	int  __fastcall k5_hello();
	int  __fastcall k5_readADC();
	int  __fastcall k5_readRSSI();
	int  __fastcall k5_reboot();

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);
	void __fastcall CMMouseEnter(TMessage &msg);
	void __fastcall CMMouseLeave(TMessage &msg);
	void __fastcall WMInitGUI(TMessage &msg);
	void __fastcall WMConnect(TMessage &msg);
	void __fastcall WMDisconnect(TMessage &msg);

protected:

	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);

		VCL_MESSAGE_HANDLER(CM_MOUSELEAVE, TMessage, CMMouseLeave);
		VCL_MESSAGE_HANDLER(CM_MOUSEENTER, TMessage, CMMouseEnter);

		VCL_MESSAGE_HANDLER(WM_INIT_GUI, TMessage, WMInitGUI);

		VCL_MESSAGE_HANDLER(WM_CONNECT, TMessage, WMConnect);
		VCL_MESSAGE_HANDLER(WM_DISCONNECT, TMessage, WMDisconnect);

	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:
	__fastcall TForm1(TComponent* Owner);
};

extern PACKAGE TForm1 *Form1;

#pragma option pop

#endif

