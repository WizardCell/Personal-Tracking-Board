import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:tracking_board_app/navigation_bar_widgets/show_tasks.dart';
import 'package:flutter/material.dart';
import 'package:tracking_board_app/screens/signin_screen.dart';
import '../navigation_bar_widgets/add_task.dart';
import '../navigation_bar_widgets/logout.dart';
import '../navigation_bar_widgets/progress.dart';
import 'package:connectivity/connectivity.dart';


class HomeScreen extends StatefulWidget {
  final String vec1;
  final Map<String, dynamic> tasks1;
  const HomeScreen({super.key,required String this.vec1, required Map<String, dynamic> this.tasks1});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {

  int _currentIndex = 0;
  late List<Widget> _widgetOptions;
  late Connectivity _connectivity;
  late bool _isConnected;

  @override
  void initState() {
    super.initState();
    _widgetOptions = <Widget>[
      ShowTasks(vec1: widget.vec1, tasks1: widget.tasks1),
      AddTask(vec1: widget.vec1, tasks1: widget.tasks1),
      Progress(vec1: widget.vec1),
      LogOut(),
    ];

    _connectivity = Connectivity();
    _isConnected = true; // Initially assume connected
    _checkConnectivity();
    _connectivity.onConnectivityChanged.listen((ConnectivityResult result) {
      setState(() {
        _isConnected = result != ConnectivityResult.none;
        if (!_isConnected && !flag) {
          _showConnectionLostMessage();
          flag = true;
        }
        if (_isConnected && flag) {
          _showConnectionRestoredMessage();
          flag = false;
        }
      });
    });
  }


  bool flag = false;
  @override
  Widget build(BuildContext context) {



    Future<int> getUsersDocumentSize() async {
      QuerySnapshot<Map<String, dynamic>> querySnapshot =
      await FirebaseFirestore.instance.collection('users').get();
      return querySnapshot.size;
    }
    getUsersDocumentSize().then((size) {
      if(size == 0){
        FirebaseAuth.instance.signOut().then((value) => {
          Navigator.push(
              context,
              MaterialPageRoute(
                  builder: (context) => const SignInScreen()))
        });
      }
    });

    return Scaffold(
      body: Center(
        child: _widgetOptions.elementAt(_currentIndex),
      ),
      bottomNavigationBar: BottomNavigationBar(
          currentIndex: _currentIndex,
          selectedItemColor: Color(0xFF4B39EF),
          unselectedItemColor: Color(0xFF57636C),
          showUnselectedLabels: true,
          selectedFontSize: 15.0,
          unselectedFontSize: 15.0,
          onTap: (index) {
            setState(() {
              _currentIndex = index;
            });
          },
          items: const [
            BottomNavigationBarItem(
              icon: Icon(
                Icons.calendar_month,
                size: 30.0,
              ),
              label: 'Tasks',
            ),
            BottomNavigationBarItem(
              icon: Icon(Icons.add, size: 30.0),
              label: 'Add Task',
            ),
            BottomNavigationBarItem(
                icon: Icon(Icons.bar_chart, size: 30.0), label: 'Progress'),
            BottomNavigationBarItem(
                icon: Icon(Icons.logout, size: 30.0), label: 'LogOut'),
          ]),
    );
  }
  void _showConnectionLostMessage() {
    // Show a Snackbar when connection is lost
    ScaffoldMessenger.of(context).showSnackBar(
      const SnackBar(
        backgroundColor: Colors.red,
        content: Text('Internet connection lost'),
        duration: Duration(seconds: 5),
      ),
    );
  }

  void _showConnectionRestoredMessage() {
    // Show a Snackbar when connection is lost
    ScaffoldMessenger.of(context).showSnackBar(
      const SnackBar(
        backgroundColor: Colors.green,
        content: Text('Internet connection restored'),
        duration: Duration(seconds: 5),
      ),
    );
  }

  Future<void> _checkConnectivity() async {
    final result = await _connectivity.checkConnectivity();
    setState(() {
      _isConnected = result != ConnectivityResult.none;
    });
  }

}



