#include <iostream>
#include <stdint.h>
#include <random>
#include <cmath>
#include <functional>
#include <cstdint>
#include <random>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <string_view>
#include <type_traits>

#include <thread>
#include <vector>
#include <algorithm>
#include <tuple>

#ifndef CPUID_H
#define CPUID_H

#ifdef _WIN32
#include <limits.h>
#include <intrin.h>
typedef unsigned __int32  uint32_t;

#else
#include <stdint.h>
#endif

class CPUID {
	uint32_t regs[4];

public:
	explicit CPUID(unsigned i) {
#ifdef _WIN32
		__cpuid((int*)regs, (int)i);
#else
		asm volatile
			("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
				: "a" (i), "c" (0));
		// ECX is set to zero for CPUID function 4
#endif
	}
	const uint32_t& EAX() const { return regs[0]; }
	const uint32_t& EBX() const { return regs[1]; }
	const uint32_t& ECX() const { return regs[2]; }
	const uint32_t& EDX() const { return regs[3]; }
};

#endif // CPUID_H

static void PrintBrand() {
	uint32_t brand[12]{};
	for (int i = 0; i < 3; i++) 
	{
		CPUID d(0x80000002 + i);
		brand[4*i + 0] = d.EAX();
		brand[4*i + 1] = d.EBX();
		brand[4*i + 2] = d.ECX();
		brand[4*i + 3] = d.EDX();
	}
	printf("%s\n", (char*)brand);
}



#define Kogge_	 (1)
#define Bob_	 (1)
#define Plain_	 (1)
#define Fancy_	 (1)
#define Pext_	 (1)
#define Hyper_	 (1)
#define Explode_ (1)
#define HypQsc_  (1)
#define HypQscNT_ (1)
#define Rotate_	 (1)
#define Arithm_	 (1)
#define Obstrd_	 (1)
#define ObstrdNT_ (1)
#define QBB_	 (1)
#define HVar_	 (1)
#define Sissy_	 (1)
#define Dumb7_	 (0)
#define Leorik_	 (1)
#define Leorik2_ (1)

#define MaskOf(X) _blsi_u64(X)
#define SquareOf(X) _tzcnt_u64(X)
#define Bitloop(X) for(;X; X = _blsr_u64(X))

#define Dummy(X) struct X {static inline constexpr std::string_view name = "dummy"; static void Prepare(uint64_t occ) {}static uint64_t Queen(int sq, uint64_t occ) { return 0; }};

static uint32_t rx = 123456789, ry = 362436069, rz = 521288629;
uint32_t rand32(void) { //2^96-1
	uint32_t t;
	rx ^= rx << 16; rx ^= rx >> 5; rx ^= rx << 1;
	t = rx; rx = ry; ry = rz; rz = t ^ rx ^ ry;
	return rz;
}
uint32_t rand32_state(uint32_t& x, uint32_t& y, uint32_t& z) { //2^96-1
	uint32_t t;
	x^= x<< 16; x^= x>> 5; x^= x<< 1;
	t = x; x= y; y = z; z = t ^ x^ y;
	return z;
}

uint64_t rand64() {
	return ((uint64_t)rand32() << 32ull) | rand32();
}

uint64_t rand64_state(uint32_t& x, uint32_t& y, uint32_t& z) {
	return (static_cast<uint64_t>(rand32_state(x,y,z)) << 32ull) | static_cast<uint64_t>(rand32_state(x,y,z));
}


template<int N>
static void ConstPrint(const char* name, uint64_t source[N])
{
	std::stringstream ss;
	ss << std::dec;
	ss << "static constexpr uint64_t " << name << "[" << N << "] = { ";
	ss << std::hex;
	for (int i = 0; i < N; i++) {
		ss << "0x" << source[i] << "ull";
		if (i != N - 1) ss << ", ";
	}
	ss << "};\n";
	ss << std::dec;
	std::cout << ss.str();
}


//Reference
#include "Switch.hpp"
struct Switch_t {
	static inline constexpr std::string_view name = "Reference";
	static inline constexpr std::string_view sp_op = "none";
	static void Prepare(uint64_t occ) {}
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Lookup_Switch::Queen(sq, occ); }
	static uint64_t Rook_Xray(int sq, uint64_t occ) { return Chess_Lookup::Lookup_Switch::Rook_Xray(sq, occ); }
	static uint64_t Bish_Xray(int sq, uint64_t occ) { return Chess_Lookup::Lookup_Switch::Bishop_Xray(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::Lookup_Switch::Size; }
};

#if Dumb7_
#include "Dumb7Fill.hpp"
struct Dumb7_t {
	static inline constexpr std::string_view name = "Dumb7 Fill";
	static inline constexpr std::string_view sp_op = "none";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Dumb7Fill::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::Dumb7Fill::Size; }
};
#else 
Dummy(Dumb7_t);
#endif


#if Kogge_
#include "KoggeStone.hpp"
struct Kogge_t {
	static inline constexpr std::string_view name = "KoggeStone";
	static inline constexpr std::string_view sp_op = "none";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::KoggeStone::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::KoggeStone::Size; }
};
#else 
	Dummy(Kogge_t);
#endif

#if Bob_
#include "Boblookup.hpp"
struct Bob_t {
	static inline constexpr std::string_view name = "BobMike";
	static inline constexpr std::string_view sp_op = "countr_zero, countl_zero";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::BobLU::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::BobLU::Size; }
};
#else 
	Dummy(Bob_t);
#endif

#if Leorik_
#include "Leorik.hpp"
	struct Leorik_t {
		static inline constexpr std::string_view name = "Leorik";
		static inline constexpr std::string_view sp_op = "countl_zero";
		static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Leorik::Queen(sq, occ); }
		static uint64_t Size() { return Chess_Lookup::Leorik::Size; }
	};
#else 
	Dummy(Leorik_);
#endif

#if Leorik2_
#include "LeorikLookup.hpp"
	struct Leorik2_t {
		static inline constexpr std::string_view name = "Leorik Lookup";
		static inline constexpr std::string_view sp_op = "countl_zero";
		static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::LeorikLookup::Queen(sq, occ); }
		static uint64_t Size() { return Chess_Lookup::LeorikLookup::Size; }
	};
#else 
	Dummy(Leorik2_);
#endif

#if Hyper_
#include "Hyperchess.hpp"
struct Hyper_t {
	static inline constexpr std::string_view name = "HyperCube";
	static inline constexpr std::string_view sp_op = "none";
	static inline void Prepare(uint64_t occ) { Chess_Lookup::Lookup_Hyper::Prepare(occ); }
	static constexpr void Move(int from, int to) { Chess_Lookup::Lookup_Hyper::Move(from, to); }
	static constexpr void Move_Take(int from, int to) { Chess_Lookup::Lookup_Hyper::Move_Take(from, to); }
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Lookup_Hyper::Queen(sq, occ); }
	static uint64_t Rook_Xray(int sq, uint64_t occ) { return Chess_Lookup::Lookup_Hyper::Rook_Xray(sq); }
	static uint64_t Bish_Xray(int sq, uint64_t occ) { return Chess_Lookup::Lookup_Hyper::Bishop_Xray(sq); }
	static uint64_t Size() { return Chess_Lookup::Lookup_Hyper::Size; }
};
#else 
	Dummy(Hyper_t);
#endif

#if Pext_
#include "Pext.hpp"
struct Pext_t {
	static inline constexpr std::string_view name = "Pext  ";
	static inline constexpr std::string_view sp_op = "pext_u64";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Lookup_Pext::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::Lookup_Pext::Size; }
};
#else 
	Dummy(Pext_t);
#endif

#if Plain_
#include "Hash_Plain.hpp"
struct Plain_t {
	static inline constexpr std::string_view name = "Hash Plain";
	static inline constexpr std::string_view sp_op = "imul64";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Plain::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::Plain::Size; }
};
#else 
Dummy(Plain_t);
#endif

#if HVar_
#include "Hash_Var.hpp"
struct HVar_t {
	static inline constexpr std::string_view name = "Hash Variable";
	static inline constexpr std::string_view sp_op = "imul64";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Var::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::Var::Size; }
};
#else 
Dummy(HVar_t);
#endif


#if Fancy_
#include "Hash_Fancy.hpp"
struct Fancy_t {
	static inline constexpr std::string_view name = "Hash Fancy";
	static inline constexpr std::string_view sp_op = "imul64";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Fancy::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::Fancy::Size; }
};
#else 
Dummy(Fancy_t);
#endif

#if Explode_
#include "Exploading.hpp"
struct Explode_t {
	static inline constexpr std::string_view name = "Exploading";
	static inline constexpr std::string_view sp_op = "imul64";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::ExplodingBoard::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::ExplodingBoard::Size; }
};
#else 
Dummy(Explode_t);
#endif

#if HypQsc_
#include "Hyperbola.hpp"
struct Hyperbola_t {
	static inline constexpr std::string_view name = "Hyperbola Qsc";
	static inline constexpr std::string_view sp_op = "bswap";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::HyperbolaQsc::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::HyperbolaQsc::Size; }
};
#else 
Dummy(Hyperbola_t);
#endif

#if HypQscNT_
#include "Hyperbola_IL.hpp"
struct HyperbolaNT_t {
	static inline constexpr std::string_view name = "Hyperb.Inline";
	static inline constexpr std::string_view sp_op = "bswap";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::HyperbolaQscInline::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::HyperbolaQscInline::Size; }
};
#else 
Dummy(HyperbolaNT_t);
#endif

#if Rotate_
#include "Rotated.hpp"
struct Rotate_t {
	static inline constexpr std::string_view name = "RotatedBoard";
	static inline constexpr std::string_view sp_op = "none";
	static inline void Prepare(uint64_t occ) { Chess_Lookup::Rotation::Prepare(occ); }
	static constexpr void Move(int from, int to) { Chess_Lookup::Rotation::Move(from, to); }
	static constexpr void Move_Take(int from, int to) { Chess_Lookup::Rotation::Move_Take(from, to); }
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::Rotation::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::Rotation::Size; }
};
#else 
Dummy(Rotate_t);
#endif


#if Arithm_
#include "SlideArithm.hpp"
struct Arithm_t {
	static inline constexpr std::string_view name = "SlideArithm";
	static inline constexpr std::string_view sp_op = "bzhi_u64, blsmsk_u64";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::SlideArithm::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::SlideArithm::Size; }
};
#else 
Dummy(Arithm_t);
#endif

#if Arithm_
#include "SlideArithm_IL.hpp"
struct ArithmNT_t {
	static inline constexpr std::string_view name = "SlideA Inline";
	static inline constexpr std::string_view sp_op = "bzhi_u64, blsmsk_u64";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::SlideArithmInline::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::SlideArithmInline::Size; }
};
#else 
Dummy(ArithmNT_t);
#endif

#if Obstrd_
#include "ObstructionDiff.hpp"
struct Obstruct_t {
	static inline constexpr std::string_view name = "Obstr. Diff";
	static inline constexpr std::string_view sp_op = "countl_zero";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::ObstructionDiff::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::ObstructionDiff::Size; }
};
#else 
Dummy(Obstruct_t);
#endif


#if ObstrdNT_
#include "ObstructionDiff_IL.hpp"
struct ObstructNT_t {
	static inline constexpr std::string_view name = "Obstr. Inline";
	static inline constexpr std::string_view sp_op = "countl_zero";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::ObstructionDiffInline::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::ObstructionDiffInline::Size; }
};
#else 
Dummy(ObstructNT_t);
#endif

#if QBB_
#include "QBB.hpp"
struct QBB_t {
	static inline constexpr std::string_view name = "QBB Algo";
	static inline constexpr std::string_view sp_op = "countr_zero, countl_zero";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::QBB::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::QBB::Size; }
};
#else 
Dummy(QBB_t);
#endif

#if Sissy_
#include "Sissy.hpp"
struct Sissy_t {
	static inline constexpr std::string_view name = "SISSY BB";
	static inline constexpr std::string_view sp_op = "none";
	static uint64_t Queen(int sq, uint64_t occ) { return Chess_Lookup::SISSY::Queen(sq, occ); }
	static uint64_t Size() { return Chess_Lookup::SISSY::Size; }
};
#else 
Dummy(Sissy_t);
#endif



static std::string _map(uint64_t value)
{
	static std::string str(64 + 8, 'o');
	for (uint64_t i = 0, c = 0; i < 64; i++)
	{
		uint64_t bitmask = (1ull) << i;

		if ((bitmask & value) != 0) str[c++] = 'X';
		else str[c++] = '.';

		if ((i + 1) % 8 == 0) str[c++] = '\n';
	}
	return str;
}


#define IsCorrect(X) if constexpr (X::name != "dummy") {if (X::Queen(i, occ) != atk_ref) { std::cout << X::name <<"failed. Reference:\n"<<"Occupy: "<<occ<<"\n"<<_map(occ)<<"\nSolution:\n"<<_map(atk_ref) <<"\nError:\n"<<_map(X::Queen(i, occ)); return false; }}


//Initialize and verify all algorithms
bool VerifyInit() {
	uint64_t occ = 0; 
	
#if Sissy_
	//Todo: Constexpr initializer
	Chess_Lookup::SISSY::Init();
#endif

	std::cout << "Verify Engines...";

	//Todo shift n bits as blockers
	for (int d = 0; d < 50000; d++) {
		if (d < 64 && d!= 0) occ = 0xffull << d;
		#if Hyper_
		   Chess_Lookup::Lookup_Hyper::Prepare(occ);
		#endif
		#if Rotate_
		   Chess_Lookup::Rotation::Prepare(occ);
		#endif

		for (int i = 0; i < 64; i++) {
			uint64_t mask = (1ull << i);
			
			if (occ & mask) {
				uint64_t atk_ref = Switch_t::Queen(i, occ);
				uint64_t xray_r = Switch_t::Rook_Xray(i, occ);
				uint64_t xray_b = Switch_t::Bish_Xray(i, occ);
				#if Hyper_
				if (Hyper_t::Rook_Xray(i, occ) != xray_r) { std::cout << Hyper_t::name << "R XRAY failed. Reference:\n" << "Occupy: " << occ << "\n" << _map(occ) << "\nSolution:\n" << _map(xray_r) << "\nError:\n" << _map(Hyper_t::Rook_Xray(i, occ)); return false; }
				if (Hyper_t::Bish_Xray(i, occ) != xray_b) { std::cout << Hyper_t::name << "B XRAY failed. Reference:\n" << "Occupy: " << occ << "\n" << _map(occ) << "\nSolution:\n" << _map(xray_b) << "\nError:\n" << _map(Hyper_t::Bish_Xray(i, occ)); return false; }
				#endif

				IsCorrect(Dumb7_t);
				IsCorrect(Kogge_t);
				IsCorrect(Bob_t);
				IsCorrect(Rotate_t);
				IsCorrect(Obstruct_t);
				IsCorrect(ObstructNT_t);
				IsCorrect(Arithm_t);
				IsCorrect(ArithmNT_t);
				IsCorrect(QBB_t);
				IsCorrect(Explode_t);
				IsCorrect(Hyperbola_t);
				IsCorrect(HyperbolaNT_t);
				IsCorrect(HVar_t);
				IsCorrect(Plain_t);
				IsCorrect(Fancy_t);
				IsCorrect(Pext_t);
				IsCorrect(Sissy_t);
				IsCorrect(Leorik_t);
				IsCorrect(Leorik2_t);
				//IsCorrect(Hyper_t);
			}
		}
		occ = rand64() & rand64();
	}
	std::cout << "OK!\n\n";
	return true;
}

const uint64_t poscnt = 1000000;
volatile uint64_t opt = 0;

//Assume 50 moves for a board with 8 sliders. (2 already off the board)
template<typename T>
double Get_MLU_EmulateGame()
{
	//Set seeds to be fair to every movegen
	rx = 123456789, ry = 362436069, rz = 521288629;

	auto t1 = std::chrono::high_resolution_clock::now();
	int actualLU = 0;
	for (int i = 0; i < poscnt / 15; i++) {
		uint64_t occ = rand64() & rand64();
		if constexpr (std::is_same<T, Hyper_t>()) {
			Hyper_t::Prepare(occ); //Set init state
		}
		if constexpr (std::is_same<T, Rotate_t>()) {
			Rotate_t::Prepare(occ); //Set init state
		}

		//128 plies
		for (int n = 0; n < 128; n++) {
			int mode = 0;
			for (int r = 0; r < 12; r++) {
				//12 lookups into the position before making a random move...
				if constexpr (std::is_same<T, Hyper_t>()) {
					opt = T::Queen(rand32() & 63, occ);
				}
				else {
					opt = T::Queen(rand32() & 63, occ);
				}
				actualLU++;
			}

			//Make a random move from -> to
			int from = rand32() & 63;
			while (!(1ull << from)) from = rand32() & 63;
			int to = rand32() & 63;
			bool taking = (occ & (1ull << to)) != 0;

			//Move now
			if constexpr (std::is_same<T, Hyper_t>()) {
				if (taking) Hyper_t::Move_Take(from, to);
				else Hyper_t::Move(from, to);
			}
			if constexpr (std::is_same<T, Rotate_t>()) {
				if (taking) Rotate_t::Move_Take(from, to);
				else Rotate_t::Move(from, to);
			}
			
			//Set occupancy
			occ &= ~(1ull << from);
			occ |= (1ull << to);
		}
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	double result = actualLU * 1000.0 / duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	std::cout << T::name << ": \t" << result << "MOps\t" << T::Size() / 1024 << " kB\t" << "Optimal perf: " << T::sp_op << "\n";
	return result;
}



template<typename T>
double Get_MLU()
{
	//Set seeds to be fair to every movegen
	rx = 123456789, ry = 362436069, rz = 521288629;

	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < poscnt; i++) {
		uint64_t occ = rand64() & rand64();
		
		if constexpr (std::is_same<T, Hyper_t>()) {
			Hyper_t::Prepare(occ);
		}
		if constexpr (std::is_same<T, Rotate_t>()) {
			Rotate_t::Prepare(occ);
		}

		for (int r = 0; r < 64; r++) {
			opt = T::Queen(r, occ);
		}
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	double result = poscnt * 64000.0 / duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	std::cout << T::name <<": \t" << result << "MOps\t"<<T::Size() / 1024 << " kB\t" << "Optimal perf: "<<T::sp_op << "\n";
	return result;
}

typedef std::tuple<std::string_view, int, double> Thread_Perf_t;

template<typename T>
Thread_Perf_t Get_MLU_Threaded()
{
	const auto processor_count = std::thread::hardware_concurrency() + 8;
	//Warmup once! - VERY important
	{
		uint32_t x = rand32();
		uint32_t y = rand32();
		uint32_t z = rand32();

		for (int n = 0; n < poscnt / 8; n++) {
			uint64_t occ = rand64_state(x, y, z) & rand64_state(x, y, z);

			if constexpr (std::is_same<T, Hyper_t>()) {
				Hyper_t::Prepare(occ);
			}
			if constexpr (std::is_same<T, Rotate_t>()) {
				Rotate_t::Prepare(occ);
			}
			for (int r = 0; r < 64; r++) {
				opt = T::Queen(r, occ);
			}
		}
	}
	double nano_ref = 0;

	//Loop of multithreaded scaling

	int bestThread = 1;
	double bestLUPerf = 0;

	std::cout << "\n" << T::name << " - " << T::Size() / 1024 << " kB\t""\n";
	std::cout << "\tthreads\tspeed\tscaling\tMegalookups/s\n";
	std::vector<std::thread> workers;
	std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> seeds;
	for (int i = 1; i <= processor_count; i++) {

		for (int I = 0; I < i; I++) seeds.push_back({ rand32(), rand32(), rand32() });

		auto t1 = std::chrono::high_resolution_clock::now();
		for (int I = 0; I < i; I++) {
			workers.push_back(std::thread([I, seeds]()
			{
				uint32_t x = std::get<0>(seeds[I]);
				uint32_t y = std::get<1>(seeds[I]);
				uint32_t z = std::get<2>(seeds[I]);
				volatile uint64_t topt = 0;

				for (int n = 0; n < poscnt / 16; n++) {
					uint64_t occ = rand64_state(x, y, z) & rand64_state(x, y, z);

					if constexpr (std::is_same<T, Hyper_t>()) {
						Hyper_t::Prepare(occ);
					}
					if constexpr (std::is_same<T, Rotate_t>()) {
						Rotate_t::Prepare(occ);
					}
					for (int r = 0; r < 64; r++) {
						topt = T::Queen(r, occ);
					}
				}
			}));
		}
		//Join all threads
		for (int I = 0; I < i; I++) workers[I].join();

		auto t2 = std::chrono::high_resolution_clock::now();
		workers.clear(); seeds.clear();
		if (i == 1) nano_ref = static_cast<double>(duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

		auto nano_current = static_cast<double>(duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

		auto MLUps_ref = 1 * poscnt / 16 * 64000.0 / nano_ref;
		auto MLUps = i * poscnt / 16 * 64000.0 / nano_current; //64000.0 = 64 lookup + nano to micro = MLUps

		auto work_factor = MLUps / MLUps_ref;
		auto scaling = work_factor / i;

		if (MLUps > bestLUPerf) {
			bestLUPerf = MLUps;
			bestThread = i;
		}

		std::cout << "\t" <<i<<"\t" << work_factor << "\t" << scaling*100 << "%\t" << MLUps << "\n";
	}

	//double result = poscnt * processor_count * 64000.0 / duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	//std::cout << T::name << ": \t" << result << "MOps\t" << T::Size() / 1024 << " kB\t" << "Optimal perf: " << T::sp_op << "\n";
	return { T::name, bestThread, bestLUPerf };
}

void PrintPerf(std::vector<Thread_Perf_t>& mt_res) {
	std::cout << "\n\nSummary:\n";

	std::sort(mt_res.begin(), mt_res.end(), [](const Thread_Perf_t& a, const Thread_Perf_t& b) -> bool
		{
			return std::get<2>(a) > std::get<2>(b);
		});

	for (auto& r : mt_res)
	{
		std::cout << std::get<0>(r) << ": \t"<<std::get<2>(r)<<"MOps \t"<<std::get<1>(r)<<"Threads\n";
	}
}

#define RunNorm(X) if constexpr (X::name != "dummy") {Get_MLU<X>(); }
#define RunMultithreaded(X) if constexpr (X::name != "dummy") { mt_res.push_back(Get_MLU_Threaded<X>()); }
#define RunEmulated(X) if constexpr (X::name != "dummy") {Get_MLU_EmulateGame<X>(); }



void GetPerf() {
	std::cout << std::setprecision(2) << std::fixed << "Megalooks Random Positions/s:\n";
	//zero tables
	RunNorm(Switch_t);
	RunNorm(Kogge_t);
	RunNorm(QBB_t);
	RunNorm(ObstructNT_t);
	RunNorm(ArithmNT_t);
	RunNorm(HyperbolaNT_t);
	RunNorm(Leorik_t);
	//small tables
	RunNorm(Leorik2_t);
	RunNorm(Arithm_t);
	RunNorm(Hyperbola_t);
	RunNorm(Explode_t);
	RunNorm(Dumb7_t);
	RunNorm(Obstruct_t);
	RunNorm(Bob_t);
	RunNorm(Rotate_t);
	//big tables
	RunNorm(Fancy_t);
	RunNorm(HVar_t);
	RunNorm(Hyper_t);
	RunNorm(Pext_t);
	RunNorm(Sissy_t);
	RunNorm(Plain_t);

	std::cout << std::setprecision(2) << std::fixed << "Megalookups Multithreaded Random Positions/s:\n";
	std::vector<Thread_Perf_t> mt_res;
	//zero tables
	RunMultithreaded(Switch_t);
	RunMultithreaded(Kogge_t);
	RunMultithreaded(QBB_t);
	RunMultithreaded(ObstructNT_t);
	RunMultithreaded(ArithmNT_t);
	RunMultithreaded(HyperbolaNT_t);
	RunMultithreaded(Leorik_t);
	//small tables
	RunMultithreaded(Leorik2_t);
	RunMultithreaded(Arithm_t);
	RunMultithreaded(Hyperbola_t);
	RunMultithreaded(Explode_t);
	RunMultithreaded(Dumb7_t);
	RunMultithreaded(Obstruct_t);
	RunMultithreaded(Bob_t);
	RunMultithreaded(Rotate_t);
	//big tables
	RunMultithreaded(Fancy_t);
	RunMultithreaded(HVar_t);
	RunMultithreaded(Hyper_t);
	RunMultithreaded(Pext_t);
	RunMultithreaded(Sissy_t);
	RunMultithreaded(Plain_t);

	
	PrintPerf(mt_res);
	
	//std::cout << std::setprecision(2) << std::fixed << "\nMegalooks Simulated Game Single Thread/ s:\n";
	//RunEmulated(Explode_t);
	//RunEmulated(Switch_t);
	//RunEmulated(Dumb7_t);
	//RunEmulated(Kogge_t);
	//RunEmulated(Rotate_t);
	//RunEmulated(QBB_t);
	//RunEmulated(Bob_t);
	//RunEmulated(Obstruct_t);
	//RunEmulated(ObstructNT_t);
	//RunEmulated(Arithm_t);
	//RunEmulated(ArithmNT_t);
	//RunEmulated(Hyperbola_t);
	//RunEmulated(HyperbolaNT_t);
	//RunEmulated(Sissy_t);
	//RunEmulated(HVar_t);
	//RunEmulated(Plain_t);
	//RunEmulated(Fancy_t);
	//RunEmulated(Pext_t);
	//RunEmulated(Hyper_t);
}

int main() {
	VerifyInit();
	PrintBrand();
	GetPerf();

	return 0;
}