#ifndef PHONEINFO_H
#define PHONEINFO_H
#include <string>

using namespace std;

class PhoneInfo
{
	private:
		string manufacturer;
		string model;
		string android_version;
		string product_name;
		int sdk_version;
	public:
		PhoneInfo()
		{
			sdk_version = -1;
		}
		PhoneInfo(const string& manufacturer, const string& model, const string& android_version, const string& product, int sdk)
		{
			this->manufacturer = manufacturer;
			this->model = model;
			this->android_version = android_version;
			this->product_name = product;
			this->sdk_version = sdk;
		}
		
		string get_manufacturer() const
		{
			return manufacturer;
		}
		
		string get_model() const
		{
			return model;
		}
		
		string get_android_version() const
		{
			return android_version;
		}
		
		string get_product_name() const
		{
			return product_name;
		}
		
		int get_sdk_version() const
		{
			return sdk_version;
		}
};

#endif // PHONEINFO_H
