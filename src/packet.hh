#include <optional>

enum class packet_type : uint8_t {
    ping
};

struct [[gnu::packed]] packet {
    packet_type pt {};
    uint64_t sequence_number = 0;
    uint64_t b;
    uint8_t a;

    packet() {
        sequence_number = next_sequence_number();
    }

    void check_sequence_number() {
        static std::optional<uint64_t> s_check = std::nullopt;
        if (!s_check) {
            s_check = this->sequence_number;
        } else {
            *s_check += 1;
            assert(this->sequence_number == *s_check);
            //fails if the UDP packets arrive out of order, or are dropped
        }
    }

    uint64_t next_sequence_number() {
        static std::optional<uint64_t> sn = std::nullopt;
        if (!sn) {
            sn = 0;
        } else {
            *sn += 1;
        }
        return *sn;
    }
};

static_assert(std::is_trivially_copyable_v<packet>);

using packet_buf = std::array<uint8_t, sizeof(packet)>;

static constexpr size_t MTU = 1500;
static_assert(sizeof(packet) < MTU);
static_assert(sizeof(packet_buf) < MTU);
