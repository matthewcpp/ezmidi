#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#include "ezmidi.h"

@interface ViewController : UIViewController{
    NSInteger selectedMidiDevice;
    NSTimer* updateTimer;
    ezmidi* ezmidi_context;
}

@property (weak, nonatomic) IBOutlet UITextField *midiDeviceText;
@property (weak, nonatomic) IBOutlet UIButton *midiConnectButton;
@property (weak, nonatomic) IBOutlet UITextView *midiEventLogTextArea;
@property (strong,nonatomic) NSMutableArray *midiDeviceList;

-(void)viewWillDisappear:(BOOL)animated;
-(NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component;
-(NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView;
-(NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component;
-(void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component;
-(void)onTick:(NSTimer *)timer;
-(void)pumpMidiEvents;
- (IBAction)onConnectButton:(id)sender;

@end

