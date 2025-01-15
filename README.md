# SimpleTester

A simple application for testing a communication protocol.
The application reads data stored in a JSON file, sends a sequence in HEX, reads and compares the received response with the expected response.

JSON file structure:

```
{
  ‘Sequences": {
    ‘Seuqence_Name_1": {
      ‘TestCase_1": {
        ‘sequence": ‘10 02 00 00 02 00 00 00 10 10 00 00 00 08 00 a7 4d 10 03’,
        ‘response": ‘10 02 00 00 02 00 00 00 05 00 02 00 08 00 42 00 56 00 00 98 28 10 03’
      },
      ‘testCase_2": {
        ‘sequence": ‘10 02 00 00 02 00 00 00 00 10 10 00 00 00 08 00 a7 4d 10 03’,
        ‘response": ‘10 02 00 00 02 00 00 00 05 00 02 00 08 00 42 00 56 00 00 98 28 10 03’
      }
    },
    ‘Seuqence_Name_2": {
      ‘TestCase_1": {
        ‘sequence": ‘10 02 00 00 02 00 00 00 00 10 10 00 00 00 08 00 a7 4d 10 03’,
        ‘response": ‘10 02 00 00 02 00 00 00 05 00 02 00 08 00 42 00 56 00 00 98 28 10 03’
      }
    }
  }
}
```
