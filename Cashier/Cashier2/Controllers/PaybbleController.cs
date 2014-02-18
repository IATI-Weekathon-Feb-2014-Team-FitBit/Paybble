using System.IO;
using System.Net;
using System.Web.Mvc;

namespace Cashier2.Controllers
{
    public class PaybbleController : Controller
    {
        private const string baseUrl = "http://paybble.azure-mobile.net/API";

        public ActionResult Default()
        {
            return View();
        }

        public ActionResult NearbyDevices(string posId)
        {
            var url = string.Format("{0}/POSGETNEARBYDEVICES?posid={1}", baseUrl, posId);
            var req = WebRequest.CreateHttp(url);
            using (var response = req.GetResponse())
            {
                using (var stream = response.GetResponseStream())
                {
                    using (var reader = new StreamReader(stream))
                    {
                        var data = reader.ReadToEnd();
                        return Content(data);
                    }
                }
            }
        }

        public ActionResult RequestPayment(string posId, string posName, string deviceId, double amount)
        {
            var url = string.Format("{0}/POSCREATEPAYMENTREQUEST?posid={1}&deviceid={2}&amount={3}&text1={4}", baseUrl, posId, deviceId, amount, posName);
            var req = WebRequest.CreateHttp(url);
            using (var response = req.GetResponse())
            {
                using (var stream = response.GetResponseStream())
                {
                    using (var reader = new StreamReader(stream))
                    {
                        var data = reader.ReadToEnd();
                        return Content(data);
                    }
                }
            }
        }

        public ActionResult PaymentStatus(string txId)
        {
            var url = string.Format("{0}/POSGETPAYMENT?transactionid={1}", baseUrl, txId);
            var req = WebRequest.CreateHttp(url);
            using (var response = req.GetResponse())
            {
                using (var stream = response.GetResponseStream())
                {
                    using (var reader = new StreamReader(stream))
                    {
                        var data = reader.ReadToEnd();
                        return Content(data);
                    }
                }
            }
        }
    }
}
