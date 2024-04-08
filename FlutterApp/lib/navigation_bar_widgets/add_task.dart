import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:tracking_board_app/reusable_widgets/reusable_widgets.dart';
import '../loading/get_vec.dart';

class AddTask extends StatefulWidget {
  final String vec1;
  final String name1;
  final Map<String, dynamic> tasks1;

  const AddTask(
      {super.key,
      required String this.vec1,
      required Map<String, dynamic> this.tasks1,
        required String this.name1});

  @override
  State<AddTask> createState() => _AddTaskState();
}

class _AddTaskState extends State<AddTask> {

  @override
  Widget build(BuildContext context) {
    String _vec = widget.vec1;
    Map<String, dynamic> _tasks = widget.tasks1;
    final TextEditingController _taskDescription = TextEditingController();
    int _day = 0;
    int _month = 0;
    int number = 0;
    return Scaffold(
      appBar: AppBar(
        title: Text(
          'Create a new task for ${widget.name1}',
          style: TextStyle(
              fontSize: 24,
              fontWeight: FontWeight.bold,
              fontFamily: 'Plus Jakarta Sans'),
        ),
      ),
      body: Center(
          child: SingleChildScrollView(
              child: Column(
                children: [

                  CalendarDatePicker(
                    initialDate: DateTime.now(),
                    firstDate: DateTime(2024),
                    lastDate: DateTime(2025),
                    onDateChanged: (date) {
                      _day = date.day;
                      _month = date.month;
                      number = _convertDateToNum(_day, _month);
                    },
                  ),
                  reusableTextField(
                      "Task description", Icons.abc, false, _taskDescription,
                      false),
                  ElevatedButton(
                    onPressed: () async {
                      if (number < _convertDateToNum(DateTime
                          .now()
                          .day, DateTime
                          .now()
                          .month)) {
                        _showErrorMessage(context,
                            "Error: This day has already passed, this action is not available.");
                      } else {
                        //vec update
                        String new_vec = "";
                        if (int.parse(_vec[number - 1]) != 9) {
                          List<String> zerosList = _vec.split('');

                          zerosList[number - 1] =
                              (int.parse(_vec[number - 1]) + 1).toString();

                          new_vec = zerosList.join('');
                        } else {
                          new_vec = _vec;
                        }

                        FirebaseFirestore.instance
                            .collection('users')
                            .doc(FirebaseAuth.instance.currentUser!.uid)
                            .update({'vec': new_vec});

                        //tasks update
                        String date1 = [_day.toString(), "/", _month.toString()]
                            .join(
                            '');
                        if (_tasks.keys.contains(date1)) {
                          _tasks[date1][_taskDescription.text] = false;
                        } else {
                          _tasks[date1] = {_taskDescription.text: false};
                        }
                        FirebaseFirestore.instance
                            .collection('users')
                            .doc(FirebaseAuth.instance.currentUser!.uid)
                            .update({'tasks': _tasks});

                        Navigator.pushReplacement(
                            context,
                            MaterialPageRoute(builder: (context) => GetVec()));
                      }
                    },
                    style: ButtonStyle(
                        backgroundColor: MaterialStateProperty.resolveWith((
                            states) {
                          if (states.contains(MaterialState.pressed)) {
                            return Colors.black26;
                          }
                          return Color(0xFF4B39EF);
                        }),
                        shape: MaterialStateProperty.all<
                            RoundedRectangleBorder>(
                            RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(12.0)))),
                    child: const Text(
                      'Submit',
                      style: TextStyle(
                          color: Color(0xFFF1F4F8),
                          fontFamily: 'Plus Jakarta Sans',
                          fontWeight: FontWeight.bold,
                          fontSize: 16),
                    ),
                  ),
                ],
              ))),
    );
  }

  int _convertDateToNum(int day, int month) {
    List<int> monthes = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
    int counter = 0;
    for (int i = 0; i < month - 1; i++) {
      counter = counter + monthes[i];
    }
    counter = counter + day;
    return counter;
  }
}

void _showErrorMessage(BuildContext context, String message) {
  ScaffoldMessenger.of(context).showSnackBar(
    SnackBar(

      content: Text(message),
      duration: Duration(seconds: 3),
    ),
  );
}




