/// Define _WIN32_WINNT as 0x0410 or higher to use WaitForSingleObject rather than Sleep() in the network thread.
/// This requires Windows 98 or higher.
/// It seems to be slower than Sleep()
/// Disabled by default.
//#define _WIN32_WINNT 0x0410

/// Define __GET_TIME_64BIT to have RakNetTime use a 64, rather than 32 bit value.  A 32 bit value will overflow after about 5 weeks.
/// However, this doubles the bandwidth use for sending times, so don't do it unless you have a reason to.
/// Disabled by default.
// #define __GET_TIME_64BIT

/// Makes RakNet threadsafe
/// Define this if you use the same instance of RakPeer from multiple threads
/// Otherwise leave it undefined, since it makes things an order of magnitude slower.
/// Disabled by default
// #define _RAKNET_THREADSAFE

/// Define __BITSTREAM_NATIVE_END to NOT support endian swapping in the BitStream class.  This is faster and is what you should use
/// unless you actually plan to have different endianness systems connect to each other
/// Enabled by default.
#define __BITSTREAM_NATIVE_END

/// Maximum (stack) size to use with _alloca before using new and delete instead.
#define MAX_ALLOCA_STACK_ALLOCATION 1000000
