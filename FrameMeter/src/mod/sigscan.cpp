#include <SigScanner/SinglePassSigScanner.hpp>

uint64_t find_function(const char *signature)
{
	uint64_t found_function = 0;

	RC::SignatureContainer signature_container{
		{{signature}},
		[&](const RC::SignatureContainer &self)
		{
			found_function = (uint64_t)self.get_match_address();
			return true;
		},
		[](RC::SignatureContainer &self) {},
	};
	RC::SinglePassScanner::SignatureContainerMap signature_containers = {
		{RC::ScanTarget::MainExe, {signature_container}},
	};
	RC::SinglePassScanner::start_scan(signature_containers);

	return found_function;
}
