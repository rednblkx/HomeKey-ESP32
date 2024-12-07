import time
import xmlrunner
import requests
import unittest
import urllib3
import urllib.parse
import sys
import argparse
import json
import os
import glob
import xml.etree.ElementTree as ET

from datetime import datetime, timedelta


class TestCaseInsights(unittest.TestCase):
    """
    Python stress tests
    """

    def __init__(self, test_name, **kwargs):
        unittest.TestCase.__init__(self, test_name)

        urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
        self.username = kwargs.get('username', None)
        self.password = kwargs.get('password', None)
        self.base_uri = kwargs.get('base_uri', None)
        self.diag_uri = kwargs.get('diag_uri', None)
        self.endpoint = kwargs.get('endpoint', '/v1/login')

        login_response = self.login()
        self.access_token = json.loads(login_response.text)["accesstoken"]
        super(unittest.TestCase, self).__init__()

    def login(self):
        """
        Login to Insights to access further APIs
        """
        header = {"content-type": "application/json"}
        uri = self.base_uri + self.endpoint
        body = json.dumps({'user_name': self.username, 'password': self.password})
        response = requests.post(url=uri, data=body, headers=header, verify=False, cookies=None)

        return response

    def test_100_get_crash_count(self):
        """
        After flashing fw, check if crashes generated are not more than 5 for 10 minutes

        - call api to get crash counts
        - Check if not more than 5 crashes are received
        - Expected : crash count shall not be more than 5.
        """
        # Arrange ------------------------------------------------------------------------
        from_ts = round(int(datetime.timestamp(datetime.now() - timedelta(minutes=1))))
        header = {"Authorization": self.access_token}
        node_id = 'eZzk4U2Xn4Aqwmitw4x6gc'

        curr_ts = round(int(datetime.timestamp(datetime.now())))
        max_time = 10*60    # duration in seconds (10 minutes)
        crash_count = 0
        filt = '[{"f":"Node.ID","o":"keyword","v":["%s"]},{"f":"Type","o":"keyword","v":["crash"]}]' % node_id
        encoded_filter = urllib.parse.quote(filt, safe='~@#$&()*!+=:;,?/\'')

        while (curr_ts-from_ts) < max_time:
            url = self.diag_uri + "/query/filters/suggest?from_ts={}&to_ts={}&filters={}&fieldname=Type".\
                format(from_ts, curr_ts, encoded_filter)
            # Act ----------------------------------------------------------------------------
            resp_crash_filter = requests.get(url=url, verify=False, headers=header, cookies=None)
            json_resp_crash_filter = json.loads(resp_crash_filter.text)["list"]
            # Assert -------------------------------------------------------------------------
            self.assertEqual(resp_crash_filter.status_code, 200, "S100.1 Unexpected status code for {} \n".format(node_id))
            # Check if any crash is present
            if len(json.loads(resp_crash_filter.text)["list"]):
                self.assertEqual(json_resp_crash_filter[0]["key"], "crash", "S100.2 key is not present for {}".format(node_id))
                self.assertLessEqual(json_resp_crash_filter[0]["count"], 5, "S100.3 Crash count is not same as expected for {}". format(node_id))
                crash_count = json_resp_crash_filter[0]["count"]
            # update variable curr_ts after every one minute
            time.sleep(60)
            curr_ts = round(int(datetime.timestamp(datetime.now())))
        print("\n Final crash count after 10 minutes is {} ".format(crash_count))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--username', default="", type=str,
                        help="username for login ")
    parser.add_argument('--password', default="", type=str,
                        help="password for login")
    parser.add_argument('--base_uri', default="", type=str,
                        help="environment login uri on which tests are to be run")
    parser.add_argument('--diag_uri', default="", type=str,
                        help="environment api uri on which tests are to be run")

    args = parser.parse_args()

    kwargs = {
        "username": args.username,
        "password": args.password,
        "base_uri": args.base_uri,
        "diag_uri": args.diag_uri,
    }

    suite = unittest.TestSuite()
    suite.addTest(TestCaseInsights('test_100_get_crash_count', **kwargs))
    xmlrunner.XMLTestRunner(verbosity=2).run(unittest.TestSuite(suite))

    # Test report generation
    for ele in (os.listdir()):
        if ele.endswith("xml"):
            os.system("junit2html {} {}.html".format(ele, os.path.splitext(ele)[0]))
            # Parse the XML report
            xml_report = ET.parse(ele)
            failures = xml_report.findall('.//failure')
            errors = xml_report.findall('.//error')
            if len(failures) > 0 or len(errors) > 0:
                print("Tests failed. see console log for details")
                sys.exit(1)

    # PATH of test reports generated by IDF unit tests
    IDF_DIR = '../../../../esp-idf/TEST_LOGS'
    xml_reports = glob.glob(os.path.join(IDF_DIR, '**/*.xml'), recursive=True)
    xml_file_path = os.path.abspath(xml_reports[0])

    if not xml_file_path:
        print("XML reports of IDF unit tests not generated")
        sys.exit(1)
    else:
        os.system("junit2html {} {}.html".format(xml_file_path, "unit_test_report"))
        # Parse the XML report
        xml_report = ET.parse(xml_file_path)
        failures = xml_report.findall('.//failure')
        errors = xml_report.findall('.//error')
        if len(failures) > 0 or len(errors) > 0:
            print("Tests failed. see console log for details")
            sys.exit(1)

    sys.exit(0)
