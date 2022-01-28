#include "router_id.hpp"
#include <bmq/base32z.h>

namespace llarp
{
  constexpr std::string_view MNODE_TLD = ".mnode";

  std::string
  RouterID::ToString() const
  {
    std::string b32 = bmq::to_base32z(begin(), end());
    b32 += MNODE_TLD;
    return b32;
  }

  std::string
  RouterID::ShortString() const
  {
    // 5 bytes produces exactly 8 base32z characters:
    return bmq::to_base32z(begin(), begin() + 5);
  }

  util::StatusObject
  RouterID::ExtractStatus() const
  {
    util::StatusObject obj{{"mnode", ToString()}, {"hex", ToHex()}};
    return obj;
  }

  bool
  RouterID::FromString(std::string_view str)
  {
    auto pos = str.find(MNODE_TLD);
    if (pos != str.size() - MNODE_TLD.size())
      return false;
    str.remove_suffix(MNODE_TLD.size());
    // Ensure we have something valid:
    // - must end in a 1-bit value: 'o' or 'y' (i.e. 10000 or 00000)
    // - must have 51 preceeding base32z chars
    // - thus we get 51*5+1 = 256 bits = 32 bytes of output
    if (str.size() != 52 || !bmq::is_base32z(str) || !(str.back() == 'o' || str.back() == 'y'))
      return false;
    bmq::from_base32z(str.begin(), str.end(), begin());
    return true;
  }
}  // namespace llarp
